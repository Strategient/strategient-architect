// Monaco Editor Loader
// This script loads Monaco from local assets or downloads it on demand

(function() {
    'use strict';

    // Check if Monaco is already loaded
    if (typeof monaco !== 'undefined') {
        if (typeof window.monacoReady === 'function') {
            window.monacoReady();
        }
        return;
    }

    // Monaco CDN fallback (for development only - production should use local assets)
    const MONACO_VERSION = '0.45.0';
    const MONACO_CDN = `https://cdn.jsdelivr.net/npm/monaco-editor@${MONACO_VERSION}/min`;

    // Configure AMD loader paths
    function configureLoader() {
        require.config({
            paths: {
                'vs': MONACO_CDN + '/vs'
            }
        });
    }

    // Load Monaco
    function loadMonaco() {
        require(['vs/editor/editor.main'], function() {
            console.log('Monaco Editor loaded successfully');
            if (typeof window.monacoReady === 'function') {
                window.monacoReady();
            }
        });
    }

    // Load AMD loader (require.js) if not present
    function loadRequireJS(callback) {
        if (typeof require !== 'undefined' && typeof require.config === 'function') {
            callback();
            return;
        }

        const script = document.createElement('script');
        script.src = MONACO_CDN + '/vs/loader.js';
        script.onload = function() {
            console.log('AMD loader loaded');
            callback();
        };
        script.onerror = function() {
            console.error('Failed to load AMD loader');
            showFallbackEditor();
        };
        document.head.appendChild(script);
    }

    // Fallback: show a simple textarea if Monaco fails to load
    function showFallbackEditor() {
        const container = document.getElementById('editor-container');
        if (!container) return;

        container.innerHTML = `
            <div style="width: 100%; height: 100%; display: flex; flex-direction: column;">
                <div style="background: #2d2d30; color: #808080; padding: 8px; font-size: 12px;">
                    Monaco Editor failed to load. Using fallback editor.
                </div>
                <textarea id="fallback-editor" style="
                    flex: 1;
                    width: 100%;
                    background: #1e1e1e;
                    color: #d4d4d4;
                    border: none;
                    padding: 8px;
                    font-family: 'JetBrains Mono', 'Fira Code', monospace;
                    font-size: 13px;
                    resize: none;
                    outline: none;
                "></textarea>
            </div>
        `;

        // Create a minimal monaco-like interface
        const textarea = document.getElementById('fallback-editor');
        window.monaco = {
            editor: {
                create: function() {
                    return {
                        getValue: () => textarea.value,
                        setValue: (v) => { textarea.value = v; },
                        onDidChangeModelContent: (cb) => {
                            textarea.addEventListener('input', () => cb());
                        },
                        updateOptions: () => {},
                        getModel: () => ({})
                    };
                },
                setModelLanguage: () => {}
            },
            languages: {
                register: () => {},
                setMonarchTokensProvider: () => {},
                setLanguageConfiguration: () => {},
                registerCompletionItemProvider: () => {},
                CompletionItemKind: { Keyword: 0, Property: 1, Snippet: 2, Operator: 3 },
                CompletionItemInsertTextRule: { InsertAsSnippet: 4 }
            }
        };

        if (typeof window.monacoReady === 'function') {
            window.monacoReady();
        }
    }

    // Initialize
    loadRequireJS(function() {
        configureLoader();
        loadMonaco();
    });

    // Timeout fallback
    setTimeout(function() {
        if (typeof monaco === 'undefined') {
            console.warn('Monaco loading timeout - using fallback');
            showFallbackEditor();
        }
    }, 10000);

})();

