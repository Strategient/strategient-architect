// PlantUML Language Definition for Monaco Editor
// Strategient Architect - Polished Implementation

/**
 * Register the PlantUML language with Monaco
 * Provides syntax highlighting and autocomplete
 */
function registerPlantUMLLanguage() {
    // ===== 1. Register Language ID =====
    monaco.languages.register({ 
        id: 'plantuml',
        extensions: ['.puml', '.plantuml', '.pu'],
        aliases: ['PlantUML', 'plantuml', 'PUML'],
        mimetypes: ['text/x-plantuml']
    });

    // ===== 2. Token Provider (Syntax Highlighting) =====
    monaco.languages.setMonarchTokensProvider('plantuml', {
        defaultToken: '',
        ignoreCase: true,
        
        // Diagram directives
        directives: [
            'startuml', 'enduml', 'startmindmap', 'endmindmap',
            'startwbs', 'endwbs', 'startgantt', 'endgantt',
            'startjson', 'endjson', 'startyaml', 'endyaml'
        ],

        // Element keywords
        elements: [
            'rectangle', 'component', 'package', 'node', 'folder', 'frame',
            'cloud', 'database', 'storage', 'queue', 'stack', 'file',
            'actor', 'usecase', 'boundary', 'control', 'entity',
            'card', 'artifact', 'hexagon', 'person', 'label',
            'collections', 'participant', 'interface', 'class',
            'abstract', 'annotation', 'enum', 'circle', 'diamond'
        ],

        // Structure keywords
        structure: [
            'title', 'caption', 'header', 'footer', 'legend', 'endlegend',
            'note', 'end note', 'hnote', 'rnote',
            'partition', 'group', 'box', 'together', 'namespace',
            'state', 'hide', 'show', 'remove', 'scale', 'rotate'
        ],

        // Flow control
        flow: [
            'if', 'then', 'else', 'elseif', 'endif',
            'while', 'endwhile', 'repeat', 'backward',
            'fork', 'again', 'end fork', 'kill', 'detach',
            'start', 'stop', 'end', 'split'
        ],

        // Modifiers
        modifiers: [
            'as', 'is', 'of', 'on', 'over',
            'left', 'right', 'up', 'down', 'top', 'bottom',
            'extends', 'implements'
        ],

        // Styling keywords
        styling: [
            'skinparam', 'style', 'sprite'
        ],

        // Arrow operators
        arrows: [
            '-->', '->',  '<--', '<-',
            '..>', '.>', '<..', '<.',
            '-->>', '->>',  '<<--', '<<-',
            '*--', 'o--', '#--', 'x--',
            '--*', '--o', '--#', '--x',
            '<|--', '--|>', '--||', '||--'
        ],

        // Symbols for matching
        symbols: /[=><!~?:&|+\-*\/\^%#]+/,

        // Escape sequences
        escapes: /\\(?:[btnfr\\"']|[0-7]{1,3}|x[0-9A-Fa-f]{2}|u[0-9A-Fa-f]{4})/,

        tokenizer: {
            root: [
                // Diagram directives (@startuml, @enduml)
                [/@\w+/, {
                    cases: {
                        '@directives': 'keyword.directive',
                        '@default': 'keyword.directive'
                    }
                }],

                // Preprocessor (!include, !define, !theme)
                [/!\w+/, 'keyword.preprocessor'],

                // Single-line comments
                [/'.*$/, 'comment'],

                // Block comments
                [/\/'/, 'comment', '@blockComment'],

                // Strings (double-quoted)
                [/"([^"\\]|\\.)*$/, 'string.invalid'],
                [/"/, 'string', '@string'],

                // Colors (#RRGGBB or #RGB)
                [/#[a-fA-F0-9]{3,8}\b/, 'constant.color'],

                // Numbers
                [/\b\d+\b/, 'number'],

                // Element and structure keywords
                [/[a-zA-Z_]\w*/, {
                    cases: {
                        '@elements': 'keyword.element',
                        '@structure': 'keyword.structure',
                        '@flow': 'keyword.flow',
                        '@modifiers': 'keyword.modifier',
                        '@styling': 'keyword.styling',
                        '@default': 'identifier'
                    }
                }],

                // Arrows and operators
                [/-->>|<<--|-->|<--|\.\.>|<\.\.|->|<-|\.>|<\./, 'operator.arrow'],
                [/\*--|o--|#--|x--/, 'operator.arrow'],
                [/--\*|--o|--#|--x/, 'operator.arrow'],
                [/<\|--|--\|>/, 'operator.arrow'],
                [/::/, 'operator'],
                [/:/, 'delimiter'],

                // Brackets
                [/[{}()\[\]]/, '@brackets'],
                [/[<>]/, '@brackets'],

                // Whitespace
                [/[ \t\r\n]+/, ''],
            ],

            blockComment: [
                [/[^']+/, 'comment'],
                [/'\//,  'comment', '@pop'],
                [/'/,    'comment']
            ],

            string: [
                [/[^\\"]+/, 'string'],
                [/@escapes/, 'string.escape'],
                [/\\./, 'string.escape.invalid'],
                [/"/, 'string', '@pop']
            ]
        }
    });

    // ===== 3. Language Configuration =====
    monaco.languages.setLanguageConfiguration('plantuml', {
        comments: {
            lineComment: "'",
            blockComment: ["/'", "'/"]
        },
        brackets: [
            ['{', '}'],
            ['[', ']'],
            ['(', ')']
        ],
        autoClosingPairs: [
            { open: '{', close: '}' },
            { open: '[', close: ']' },
            { open: '(', close: ')' },
            { open: '"', close: '"' },
            { open: "'", close: "'" }
        ],
        surroundingPairs: [
            { open: '{', close: '}' },
            { open: '[', close: ']' },
            { open: '(', close: ')' },
            { open: '"', close: '"' }
        ],
        folding: {
            markers: {
                start: /^\s*(@startuml|package|rectangle|node|cloud|database|partition|group|box)/i,
                end: /^\s*(@enduml|})/i
            }
        },
        indentationRules: {
            increaseIndentPattern: /^\s*(package|rectangle|node|folder|frame|cloud|database|partition|group|box|together|namespace)\s+.*\{\s*$/i,
            decreaseIndentPattern: /^\s*\}\s*$/
        }
    });

    // ===== 4. Completion Provider (Autocomplete) =====
    monaco.languages.registerCompletionItemProvider('plantuml', {
        triggerCharacters: ['@', '!', '-', '.', '<'],
        
        provideCompletionItems: function(model, position) {
            const word = model.getWordUntilPosition(position);
            const range = {
                startLineNumber: position.lineNumber,
                endLineNumber: position.lineNumber,
                startColumn: word.startColumn,
                endColumn: word.endColumn
            };

            // Get line text for context
            const lineText = model.getLineContent(position.lineNumber);
            const textBefore = lineText.substring(0, position.column - 1);

            const suggestions = [];
            let sortPriority = 0;

            // Helper to add suggestion with priority
            const addSuggestion = (label, kind, insertText, detail, isSnippet = false) => {
                suggestions.push({
                    label: label,
                    kind: kind,
                    insertText: insertText,
                    insertTextRules: isSnippet ? 
                        monaco.languages.CompletionItemInsertTextRule.InsertAsSnippet : undefined,
                    detail: detail,
                    range: range,
                    sortText: String(sortPriority++).padStart(3, '0')
                });
            };

            // === Priority 1: Diagram Structure (most common) ===
            addSuggestion('@startuml', monaco.languages.CompletionItemKind.Keyword,
                '@startuml ${1:diagram_name}\n\n$0\n\n@enduml',
                'Start a new PlantUML diagram', true);
            
            addSuggestion('@enduml', monaco.languages.CompletionItemKind.Keyword,
                '@enduml', 'End the PlantUML diagram');
            
            addSuggestion('title', monaco.languages.CompletionItemKind.Keyword,
                'title ${1:Diagram Title}', 'Set diagram title', true);

            // === Priority 2: Common Elements ===
            addSuggestion('rectangle', monaco.languages.CompletionItemKind.Keyword,
                'rectangle "${1:Name}" as ${2:ALIAS} {\n  $0\n}',
                'Container rectangle', true);
            
            addSuggestion('component', monaco.languages.CompletionItemKind.Keyword,
                'component "${1:Name}" as ${2:ALIAS}',
                'Component element', true);
            
            addSuggestion('package', monaco.languages.CompletionItemKind.Keyword,
                'package "${1:Name}" as ${2:ALIAS} {\n  $0\n}',
                'Package container', true);
            
            addSuggestion('database', monaco.languages.CompletionItemKind.Keyword,
                'database "${1:Name}" as ${2:ALIAS}',
                'Database element', true);
            
            addSuggestion('cloud', monaco.languages.CompletionItemKind.Keyword,
                'cloud "${1:Name}" as ${2:ALIAS} {\n  $0\n}',
                'Cloud container', true);
            
            addSuggestion('node', monaco.languages.CompletionItemKind.Keyword,
                'node "${1:Name}" as ${2:ALIAS} {\n  $0\n}',
                'Node container', true);

            // === Priority 3: Other Elements ===
            addSuggestion('actor', monaco.languages.CompletionItemKind.Keyword,
                'actor "${1:Name}" as ${2:ALIAS}',
                'Actor element', true);
            
            addSuggestion('usecase', monaco.languages.CompletionItemKind.Keyword,
                'usecase "${1:Name}" as ${2:ALIAS}',
                'Use case element', true);
            
            addSuggestion('storage', monaco.languages.CompletionItemKind.Keyword,
                'storage "${1:Name}" as ${2:ALIAS}',
                'Storage element', true);
            
            addSuggestion('queue', monaco.languages.CompletionItemKind.Keyword,
                'queue "${1:Name}" as ${2:ALIAS}',
                'Queue element', true);
            
            addSuggestion('folder', monaco.languages.CompletionItemKind.Keyword,
                'folder "${1:Name}" as ${2:ALIAS} {\n  $0\n}',
                'Folder container', true);
            
            addSuggestion('frame', monaco.languages.CompletionItemKind.Keyword,
                'frame "${1:Name}" as ${2:ALIAS} {\n  $0\n}',
                'Frame container', true);

            // === Priority 4: Notes ===
            addSuggestion('note left', monaco.languages.CompletionItemKind.Snippet,
                'note left of ${1:element}\n  ${2:Note text}\nend note',
                'Note on left side', true);
            
            addSuggestion('note right', monaco.languages.CompletionItemKind.Snippet,
                'note right of ${1:element}\n  ${2:Note text}\nend note',
                'Note on right side', true);

            // === Priority 5: Arrows ===
            addSuggestion('-->', monaco.languages.CompletionItemKind.Operator,
                '-->', 'Solid arrow');
            
            addSuggestion('..>', monaco.languages.CompletionItemKind.Operator,
                '..>', 'Dotted arrow');
            
            addSuggestion('->', monaco.languages.CompletionItemKind.Operator,
                '->', 'Short solid arrow');
            
            addSuggestion('<--', monaco.languages.CompletionItemKind.Operator,
                '<--', 'Reverse solid arrow');

            // === Priority 6: Skinparam ===
            addSuggestion('skinparam', monaco.languages.CompletionItemKind.Keyword,
                'skinparam ${1:property} ${2:value}',
                'Set diagram styling', true);

            return { suggestions: suggestions };
        }
    });
}

/**
 * Define the Strategient dark theme for Monaco
 */
function defineStrategientTheme() {
    monaco.editor.defineTheme('strategient-dark', {
        base: 'vs-dark',
        inherit: true,
        rules: [
            // Comments - muted green
            { token: 'comment', foreground: '6A9955', fontStyle: 'italic' },
            
            // Strings - warm orange
            { token: 'string', foreground: 'CE9178' },
            { token: 'string.escape', foreground: 'D7BA7D' },
            { token: 'string.invalid', foreground: 'F44747' },
            
            // Keywords and directives - soft blue
            { token: 'keyword', foreground: '569CD6' },
            { token: 'keyword.directive', foreground: 'C586C0', fontStyle: 'bold' },
            { token: 'keyword.preprocessor', foreground: 'C586C0' },
            { token: 'keyword.element', foreground: '4EC9B0' },
            { token: 'keyword.structure', foreground: '569CD6' },
            { token: 'keyword.flow', foreground: 'C586C0' },
            { token: 'keyword.modifier', foreground: '9CDCFE' },
            { token: 'keyword.styling', foreground: 'DCDCAA' },
            
            // Operators and arrows - light gray
            { token: 'operator', foreground: 'D4D4D4' },
            { token: 'operator.arrow', foreground: '808080' },
            
            // Identifiers - default text
            { token: 'identifier', foreground: 'D4D4D4' },
            
            // Numbers - light green
            { token: 'number', foreground: 'B5CEA8' },
            
            // Colors - actual color or gold
            { token: 'constant.color', foreground: 'DCDCAA' },
            
            // Delimiters
            { token: 'delimiter', foreground: 'D4D4D4' },
            { token: '@brackets', foreground: 'FFD700' }
        ],
        colors: {
            // Editor background - dark but not pure black
            'editor.background': '#1E1E1E',
            'editor.foreground': '#D4D4D4',
            
            // Gutter
            'editorLineNumber.foreground': '#5A5A5A',
            'editorLineNumber.activeForeground': '#C6C6C6',
            'editorGutter.background': '#1E1E1E',
            
            // Selection
            'editor.selectionBackground': '#264F78',
            'editor.selectionHighlightBackground': '#3A3D41',
            'editor.inactiveSelectionBackground': '#3A3D41',
            
            // Current line
            'editor.lineHighlightBackground': '#2A2D2E',
            'editor.lineHighlightBorder': '#2A2D2E',
            
            // Cursor
            'editorCursor.foreground': '#AEAFAD',
            
            // Whitespace
            'editorWhitespace.foreground': '#3B3B3B',
            
            // Indent guides
            'editorIndentGuide.background': '#404040',
            'editorIndentGuide.activeBackground': '#707070',
            
            // Scrollbar
            'scrollbarSlider.background': '#4E4E4E80',
            'scrollbarSlider.hoverBackground': '#646464B0',
            'scrollbarSlider.activeBackground': '#6E6E6EE0',
            
            // Widget (autocomplete dropdown)
            'editorWidget.background': '#252526',
            'editorWidget.border': '#454545',
            'editorSuggestWidget.background': '#252526',
            'editorSuggestWidget.border': '#454545',
            'editorSuggestWidget.selectedBackground': '#094771',
            'editorSuggestWidget.highlightForeground': '#0097FB',
            
            // Find/replace
            'editor.findMatchBackground': '#515C6A',
            'editor.findMatchHighlightBackground': '#EA5C0055',
            
            // Minimap
            'minimap.background': '#1E1E1E',
            'minimapSlider.background': '#4E4E4E40',
            
            // Bracket matching
            'editorBracketMatch.background': '#0064001A',
            'editorBracketMatch.border': '#888888'
        }
    });
}
