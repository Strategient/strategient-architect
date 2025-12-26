// PlantUML language definition for Monaco Editor

function registerPlantUMLLanguage() {
    // Register a new language
    monaco.languages.register({ id: 'plantuml' });

    // Define token provider (syntax highlighting)
    monaco.languages.setMonarchTokensProvider('plantuml', {
        defaultToken: '',
        ignoreCase: true,

        keywords: [
            'startuml', 'enduml', 'startmindmap', 'endmindmap',
            'startwbs', 'endwbs', 'startgantt', 'endgantt',
            'title', 'caption', 'header', 'footer', 'legend',
            'note', 'end note', 'hnote', 'rnote',
            'left', 'right', 'top', 'bottom', 'of', 'on', 'over',
            'if', 'then', 'else', 'elseif', 'endif',
            'while', 'endwhile', 'repeat', 'backward',
            'fork', 'again', 'end fork', 'kill', 'detach',
            'start', 'stop', 'end',
            'partition', 'group', 'box', 'together',
            'package', 'namespace', 'node', 'folder', 'frame',
            'cloud', 'database', 'storage', 'queue', 'stack',
            'rectangle', 'hexagon', 'card', 'file', 'artifact',
            'component', 'interface', 'usecase', 'actor',
            'class', 'abstract', 'annotation', 'enum', 'entity',
            'circle', 'diamond',
            'state', 'choice', 'hide', 'show', 'remove',
            'skinparam', 'style', 'theme',
            'sprite', 'scale', 'rotate',
            'up', 'down', 'left', 'right',
            'as', 'is', 'extends', 'implements'
        ],

        operators: [
            '->', '-->', '->>', '-->>', '<-', '<--', '<<-', '<<--',
            '-[', ']-', '.>', '..>', '<.', '<..', '*--', 'o--',
            '#--', 'x--', '}--', '|--', '+--', '^--',
            '--|>', '--*', '--o', '--#', '--x', '--|', '--+', '--^',
            '<|--', '*--', 'o--', '#--', 'x--', '|--', '+--', '^--',
            '..', '--', '~~', '==', '::', ':', '|'
        ],

        symbols: /[=><!~?:&|+\-*\/\^%#@]+/,

        tokenizer: {
            root: [
                // Comments
                [/'.*$/, 'comment'],
                [/\/\'/, 'comment', '@comment'],
                
                // Directives
                [/@[a-zA-Z_]\w*/, 'keyword'],
                [/![a-zA-Z_]\w*/, 'keyword.directive'],
                
                // Strings
                [/"([^"\\]|\\.)*$/, 'string.invalid'],
                [/"/, 'string', '@string'],
                
                // Keywords and identifiers
                [/[a-zA-Z_]\w*/, {
                    cases: {
                        '@keywords': 'keyword',
                        '@default': 'identifier'
                    }
                }],
                
                // Whitespace
                [/[ \t\r\n]+/, ''],
                
                // Delimiters
                [/[{}()\[\]]/, '@brackets'],
                [/[<>](?!@symbols)/, '@brackets'],
                
                // Operators
                [/@symbols/, {
                    cases: {
                        '@operators': 'operator',
                        '@default': ''
                    }
                }],
                
                // Numbers
                [/\d+/, 'number'],
                
                // Colors
                [/#[a-fA-F0-9]{3,8}/, 'constant.color']
            ],
            
            comment: [
                [/[^']+/, 'comment'],
                [/\'\//, 'comment', '@pop'],
                [/'/, 'comment']
            ],
            
            string: [
                [/[^\\"]+/, 'string'],
                [/\\./, 'string.escape'],
                [/"/, 'string', '@pop']
            ]
        }
    });

    // Define language configuration (brackets, comments, etc.)
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
            { open: '"', close: '"' }
        ],
        surroundingPairs: [
            { open: '{', close: '}' },
            { open: '[', close: ']' },
            { open: '(', close: ')' },
            { open: '"', close: '"' }
        ],
        folding: {
            markers: {
                start: /^\s*@start/,
                end: /^\s*@end/
            }
        }
    });

    // Register completion provider
    monaco.languages.registerCompletionItemProvider('plantuml', {
        provideCompletionItems: function(model, position) {
            const word = model.getWordUntilPosition(position);
            const range = {
                startLineNumber: position.lineNumber,
                endLineNumber: position.lineNumber,
                startColumn: word.startColumn,
                endColumn: word.endColumn
            };

            const suggestions = [
                // Diagram types
                { label: '@startuml', kind: monaco.languages.CompletionItemKind.Keyword, insertText: '@startuml ${1:diagram_name}\n\n$0\n\n@enduml', insertTextRules: monaco.languages.CompletionItemInsertTextRule.InsertAsSnippet, range: range },
                { label: '@enduml', kind: monaco.languages.CompletionItemKind.Keyword, insertText: '@enduml', range: range },
                
                // Elements
                { label: 'rectangle', kind: monaco.languages.CompletionItemKind.Keyword, insertText: 'rectangle "${1:Name}" as ${2:ALIAS} {\n  $0\n}', insertTextRules: monaco.languages.CompletionItemInsertTextRule.InsertAsSnippet, range: range },
                { label: 'component', kind: monaco.languages.CompletionItemKind.Keyword, insertText: 'component "${1:Name}" as ${2:ALIAS}', insertTextRules: monaco.languages.CompletionItemInsertTextRule.InsertAsSnippet, range: range },
                { label: 'node', kind: monaco.languages.CompletionItemKind.Keyword, insertText: 'node "${1:Name}" as ${2:ALIAS} {\n  $0\n}', insertTextRules: monaco.languages.CompletionItemInsertTextRule.InsertAsSnippet, range: range },
                { label: 'database', kind: monaco.languages.CompletionItemKind.Keyword, insertText: 'database "${1:Name}" as ${2:ALIAS}', insertTextRules: monaco.languages.CompletionItemInsertTextRule.InsertAsSnippet, range: range },
                { label: 'package', kind: monaco.languages.CompletionItemKind.Keyword, insertText: 'package "${1:Name}" as ${2:ALIAS} {\n  $0\n}', insertTextRules: monaco.languages.CompletionItemInsertTextRule.InsertAsSnippet, range: range },
                { label: 'cloud', kind: monaco.languages.CompletionItemKind.Keyword, insertText: 'cloud "${1:Name}" as ${2:ALIAS} {\n  $0\n}', insertTextRules: monaco.languages.CompletionItemInsertTextRule.InsertAsSnippet, range: range },
                { label: 'actor', kind: monaco.languages.CompletionItemKind.Keyword, insertText: 'actor "${1:Name}" as ${2:ALIAS}', insertTextRules: monaco.languages.CompletionItemInsertTextRule.InsertAsSnippet, range: range },
                { label: 'usecase', kind: monaco.languages.CompletionItemKind.Keyword, insertText: 'usecase "${1:Name}" as ${2:ALIAS}', insertTextRules: monaco.languages.CompletionItemInsertTextRule.InsertAsSnippet, range: range },
                { label: 'storage', kind: monaco.languages.CompletionItemKind.Keyword, insertText: 'storage "${1:Name}" as ${2:ALIAS}', insertTextRules: monaco.languages.CompletionItemInsertTextRule.InsertAsSnippet, range: range },
                { label: 'queue', kind: monaco.languages.CompletionItemKind.Keyword, insertText: 'queue "${1:Name}" as ${2:ALIAS}', insertTextRules: monaco.languages.CompletionItemInsertTextRule.InsertAsSnippet, range: range },
                
                // Styling
                { label: 'skinparam', kind: monaco.languages.CompletionItemKind.Keyword, insertText: 'skinparam ${1:property} ${2:value}', insertTextRules: monaco.languages.CompletionItemInsertTextRule.InsertAsSnippet, range: range },
                { label: 'skinparam backgroundColor', kind: monaco.languages.CompletionItemKind.Property, insertText: 'skinparam backgroundColor #${1:ffffff}', insertTextRules: monaco.languages.CompletionItemInsertTextRule.InsertAsSnippet, range: range },
                { label: 'skinparam componentStyle', kind: monaco.languages.CompletionItemKind.Property, insertText: 'skinparam componentStyle ${1|rectangle,uml|}', insertTextRules: monaco.languages.CompletionItemInsertTextRule.InsertAsSnippet, range: range },
                { label: 'title', kind: monaco.languages.CompletionItemKind.Keyword, insertText: 'title ${1:Title}', insertTextRules: monaco.languages.CompletionItemInsertTextRule.InsertAsSnippet, range: range },
                
                // Notes
                { label: 'note left', kind: monaco.languages.CompletionItemKind.Snippet, insertText: 'note left of ${1:element}\n  ${2:text}\nend note', insertTextRules: monaco.languages.CompletionItemInsertTextRule.InsertAsSnippet, range: range },
                { label: 'note right', kind: monaco.languages.CompletionItemKind.Snippet, insertText: 'note right of ${1:element}\n  ${2:text}\nend note', insertTextRules: monaco.languages.CompletionItemInsertTextRule.InsertAsSnippet, range: range },
                
                // Arrows
                { label: '-->', kind: monaco.languages.CompletionItemKind.Operator, insertText: '-->', range: range },
                { label: '..>', kind: monaco.languages.CompletionItemKind.Operator, insertText: '..>', range: range },
                { label: '->', kind: monaco.languages.CompletionItemKind.Operator, insertText: '->', range: range }
            ];

            return { suggestions: suggestions };
        }
    });
}

