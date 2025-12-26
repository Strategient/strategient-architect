// DOT/Graphviz Language Definition for Monaco Editor
// Strategient Architect - Graphviz Integration

/**
 * Register the DOT language with Monaco
 * Provides comprehensive syntax highlighting and autocomplete for Graphviz DOT files
 */
function registerDotLanguage() {
    // ===== 1. Register Language ID =====
    monaco.languages.register({ 
        id: 'dot',
        extensions: ['.dot', '.gv', '.graphviz'],
        aliases: ['DOT', 'Graphviz', 'dot', 'gv'],
        mimetypes: ['text/vnd.graphviz', 'text/x-dot']
    });

    // ===== 2. Token Provider (Syntax Highlighting) =====
    monaco.languages.setMonarchTokensProvider('dot', {
        defaultToken: '',
        ignoreCase: true,
        
        // Graph type keywords
        graphTypes: ['digraph', 'graph', 'subgraph', 'strict'],

        // Attribute keywords
        attributeKeywords: [
            'node', 'edge', 'graph'
        ],

        // Common graph attributes
        graphAttributes: [
            'bgcolor', 'center', 'charset', 'clusterrank', 'colorscheme',
            'compound', 'concentrate', 'dpi', 'fontcolor', 'fontname',
            'fontsize', 'forcelabels', 'id', 'label', 'labeljust',
            'labelloc', 'landscape', 'layers', 'layersep', 'layout',
            'margin', 'mindist', 'newrank', 'nodesep', 'nojustify',
            'ordering', 'orientation', 'outputorder', 'overlap', 'pack',
            'packmode', 'pad', 'page', 'pagedir', 'quantum', 'rankdir',
            'ranksep', 'ratio', 'remincross', 'rotate', 'searchsize',
            'sep', 'showboxes', 'size', 'sortv', 'splines', 'start',
            'stylesheet', 'target', 'truecolor', 'viewport', 'voro_margin'
        ],

        // Common node attributes
        nodeAttributes: [
            'color', 'colorscheme', 'comment', 'distortion', 'fillcolor',
            'fixedsize', 'fontcolor', 'fontname', 'fontsize', 'gradientangle',
            'group', 'height', 'href', 'id', 'image', 'imagepos', 'imagescale',
            'label', 'labelloc', 'layer', 'margin', 'nojustify', 'ordering',
            'orientation', 'penwidth', 'peripheries', 'pin', 'pos', 'rects',
            'regular', 'root', 'samplepoints', 'shape', 'shapefile', 'showboxes',
            'sides', 'skew', 'sortv', 'style', 'target', 'tooltip', 'url',
            'vertices', 'width', 'xlabel', 'xlp', 'z'
        ],

        // Common edge attributes
        edgeAttributes: [
            'arrowhead', 'arrowsize', 'arrowtail', 'color', 'colorscheme',
            'comment', 'constraint', 'decorate', 'dir', 'edgehref', 'edgetarget',
            'edgetooltip', 'edgeurl', 'fillcolor', 'fontcolor', 'fontname',
            'fontsize', 'headclip', 'headhref', 'headlabel', 'headport',
            'headtarget', 'headtooltip', 'headurl', 'href', 'id', 'label',
            'labelangle', 'labeldistance', 'labelfloat', 'labelfontcolor',
            'labelfontname', 'labelfontsize', 'labelhref', 'labeltarget',
            'labeltooltip', 'labelurl', 'layer', 'len', 'lhead', 'lp', 'ltail',
            'minlen', 'nojustify', 'penwidth', 'pos', 'samehead', 'sametail',
            'showboxes', 'style', 'tailclip', 'tailhref', 'taillabel', 'tailport',
            'tailtarget', 'tailtooltip', 'tailurl', 'target', 'tooltip', 'url',
            'weight', 'xlabel', 'xlp'
        ],

        // Shape values
        shapes: [
            'box', 'polygon', 'ellipse', 'oval', 'circle', 'point', 'egg',
            'triangle', 'plaintext', 'plain', 'diamond', 'trapezium',
            'parallelogram', 'house', 'pentagon', 'hexagon', 'septagon',
            'octagon', 'doublecircle', 'doubleoctagon', 'tripleoctagon',
            'invtriangle', 'invtrapezium', 'invhouse', 'Mdiamond', 'Msquare',
            'Mcircle', 'rect', 'rectangle', 'square', 'star', 'none',
            'underline', 'cylinder', 'note', 'tab', 'folder', 'box3d',
            'component', 'promoter', 'cds', 'terminator', 'utr', 'primersite',
            'restrictionsite', 'fivepoverhang', 'threepoverhang', 'noverhang',
            'assembly', 'signature', 'insulator', 'ribosite', 'rnastab',
            'proteasesite', 'proteinstab', 'rpromoter', 'rarrow', 'larrow',
            'lpromoter', 'record', 'Mrecord'
        ],

        // Arrow types
        arrowTypes: [
            'normal', 'inv', 'dot', 'invdot', 'odot', 'invodot', 'none',
            'tee', 'empty', 'invempty', 'diamond', 'odiamond', 'ediamond',
            'crow', 'box', 'obox', 'open', 'halfopen', 'vee'
        ],

        // Style values
        styles: [
            'solid', 'dashed', 'dotted', 'bold', 'rounded', 'diagonals',
            'filled', 'striped', 'wedged', 'invis', 'tapered'
        ],

        // Direction values
        directions: ['forward', 'back', 'both', 'none'],

        // Rankdir values
        rankdirs: ['TB', 'BT', 'LR', 'RL'],

        // Splines values
        splinesValues: ['none', 'line', 'polyline', 'curved', 'ortho', 'spline', 'true', 'false'],

        // Compass points for ports
        compassPoints: ['n', 'ne', 'e', 'se', 's', 'sw', 'w', 'nw', 'c', '_'],

        // Symbols
        symbols: /[=><!~?:&|+\-*\/\^%]+/,

        // Tokenizer rules
        tokenizer: {
            root: [
                // Comments: // or /* */
                [/\/\/.*$/, 'comment'],
                [/\/\*/, 'comment', '@blockComment'],
                
                // C-style comments: # 
                [/#.*$/, 'comment'],
                
                // Graph types (digraph, graph, subgraph, strict)
                [/\b(strict)\b/i, 'keyword.modifier'],
                [/\b(digraph|graph|subgraph)\b/i, 'keyword.graph'],
                
                // Attribute target keywords (node, edge, graph as attribute target)
                [/\b(node|edge|graph)\s*\[/, { token: 'keyword.attribute', next: '@attributes' }],
                
                // HTML-like labels
                [/</, { token: 'string.html', next: '@htmlLabel', bracket: '@open' }],
                
                // Strings (double-quoted)
                [/"/, 'string', '@string'],
                
                // Colors (#RRGGBB or #RRGGBBAA or named colors)
                [/#[a-fA-F0-9]{6,8}\b/, 'constant.color'],
                [/#[a-fA-F0-9]{3}\b/, 'constant.color'],
                
                // Numbers (including decimals and negative)
                [/-?\d+\.?\d*/, 'number'],
                
                // Attribute names (followed by =)
                [/[a-zA-Z_][a-zA-Z0-9_]*(?=\s*=)/, 'attribute.name'],
                
                // Node/edge identifiers
                [/[a-zA-Z_][a-zA-Z0-9_]*/, 'identifier'],
                
                // Edge operators
                [/->/, 'operator.edge'],
                [/--/, 'operator.edge'],
                
                // Brackets
                [/[{}()\[\]]/, '@brackets'],
                
                // Punctuation
                [/[;,:]/, 'delimiter'],
                [/=/, 'operator'],
                
                // Whitespace
                [/[ \t\r\n]+/, ''],
            ],

            blockComment: [
                [/[^/*]+/, 'comment'],
                [/\*\//, 'comment', '@pop'],
                [/[/*]/, 'comment']
            ],

            string: [
                [/[^\\"]+/, 'string'],
                [/\\./, 'string.escape'],
                [/"/, 'string', '@pop']
            ],

            htmlLabel: [
                [/>/, { token: 'string.html', next: '@pop', bracket: '@close' }],
                [/<\//, { token: 'string.html.tag', next: '@htmlCloseTag' }],
                [/</, { token: 'string.html.tag', next: '@htmlOpenTag' }],
                [/[^<>]+/, 'string.html']
            ],

            htmlOpenTag: [
                [/[a-zA-Z_][a-zA-Z0-9_]*/, 'string.html.tag'],
                [/=/, 'operator'],
                [/"[^"]*"/, 'string'],
                [/>/, { token: 'string.html.tag', next: '@htmlLabel' }],
                [/\/>/, { token: 'string.html.tag', next: '@pop' }],
                [/[ \t]+/, '']
            ],

            htmlCloseTag: [
                [/[a-zA-Z_][a-zA-Z0-9_]*/, 'string.html.tag'],
                [/>/, { token: 'string.html.tag', next: '@htmlLabel' }],
                [/[ \t]+/, '']
            ],

            attributes: [
                [/\]/, { token: '@brackets', next: '@pop' }],
                [/"/, 'string', '@string'],
                [/#[a-fA-F0-9]{6,8}\b/, 'constant.color'],
                [/#[a-fA-F0-9]{3}\b/, 'constant.color'],
                [/-?\d+\.?\d*/, 'number'],
                [/[a-zA-Z_][a-zA-Z0-9_]*(?=\s*=)/, 'attribute.name'],
                [/[a-zA-Z_][a-zA-Z0-9_]*/, 'attribute.value'],
                [/=/, 'operator'],
                [/[,;]/, 'delimiter'],
                [/[ \t\r\n]+/, '']
            ]
        }
    });

    // ===== 3. Language Configuration =====
    monaco.languages.setLanguageConfiguration('dot', {
        comments: {
            lineComment: '//',
            blockComment: ['/*', '*/']
        },
        brackets: [
            ['{', '}'],
            ['[', ']'],
            ['(', ')'],
            ['<', '>']
        ],
        autoClosingPairs: [
            { open: '{', close: '}' },
            { open: '[', close: ']' },
            { open: '(', close: ')' },
            { open: '"', close: '"' },
            { open: '<', close: '>' }
        ],
        surroundingPairs: [
            { open: '{', close: '}' },
            { open: '[', close: ']' },
            { open: '(', close: ')' },
            { open: '"', close: '"' },
            { open: '<', close: '>' }
        ],
        folding: {
            markers: {
                start: /^\s*(digraph|graph|subgraph)\s+[^{]*\{/i,
                end: /^\s*\}/
            }
        },
        indentationRules: {
            increaseIndentPattern: /^.*\{\s*$/,
            decreaseIndentPattern: /^\s*\}\s*$/
        }
    });

    // ===== 4. Completion Provider (Autocomplete) =====
    monaco.languages.registerCompletionItemProvider('dot', {
        triggerCharacters: ['[', '=', ' ', '\n'],
        
        provideCompletionItems: function(model, position) {
            const word = model.getWordUntilPosition(position);
            const range = {
                startLineNumber: position.lineNumber,
                endLineNumber: position.lineNumber,
                startColumn: word.startColumn,
                endColumn: word.endColumn
            };

            const lineText = model.getLineContent(position.lineNumber);
            const textBefore = lineText.substring(0, position.column - 1);

            const suggestions = [];
            let sortPriority = 0;

            const addSuggestion = (label, kind, insertText, detail, isSnippet = false, docString = '') => {
                suggestions.push({
                    label: label,
                    kind: kind,
                    insertText: insertText,
                    insertTextRules: isSnippet ? 
                        monaco.languages.CompletionItemInsertTextRule.InsertAsSnippet : undefined,
                    detail: detail,
                    documentation: docString || undefined,
                    range: range,
                    sortText: String(sortPriority++).padStart(3, '0')
                });
            };

            // Detect context
            const isInAttributes = /\[\s*$/.test(textBefore) || /,\s*$/.test(textBefore) || 
                                   (/\[/.test(textBefore) && !/\]/.test(textBefore));
            const isAfterEquals = /=\s*$/.test(textBefore);
            const isShapeContext = /shape\s*=\s*$/.test(textBefore);
            const isStyleContext = /style\s*=\s*$/.test(textBefore);
            const isArrowContext = /(arrowhead|arrowtail)\s*=\s*$/.test(textBefore);
            const isDirContext = /dir\s*=\s*$/.test(textBefore);
            const isRankdirContext = /rankdir\s*=\s*$/.test(textBefore);
            const isSplinesContext = /splines\s*=\s*$/.test(textBefore);

            // === Context-specific completions ===
            if (isShapeContext) {
                // Shape values
                const shapes = [
                    'box', 'ellipse', 'circle', 'diamond', 'rectangle', 'square',
                    'cylinder', 'note', 'folder', 'component', 'tab', 'box3d',
                    'doublecircle', 'doubleoctagon', 'tripleoctagon',
                    'hexagon', 'octagon', 'pentagon', 'septagon',
                    'triangle', 'invtriangle', 'trapezium', 'invtrapezium',
                    'parallelogram', 'house', 'invhouse', 'star', 'none',
                    'record', 'Mrecord', 'plaintext', 'plain', 'point'
                ];
                shapes.forEach(shape => {
                    addSuggestion(shape, monaco.languages.CompletionItemKind.EnumMember,
                        shape, `Node shape: ${shape}`);
                });
                return { suggestions };
            }

            if (isStyleContext) {
                const styles = ['solid', 'dashed', 'dotted', 'bold', 'rounded', 
                               'filled', 'striped', 'wedged', 'invis', 'diagonals', 'tapered'];
                styles.forEach(style => {
                    addSuggestion(style, monaco.languages.CompletionItemKind.EnumMember,
                        style, `Style: ${style}`);
                });
                return { suggestions };
            }

            if (isArrowContext) {
                const arrows = ['normal', 'inv', 'dot', 'invdot', 'odot', 'none',
                               'tee', 'empty', 'diamond', 'crow', 'box', 'vee', 'open'];
                arrows.forEach(arrow => {
                    addSuggestion(arrow, monaco.languages.CompletionItemKind.EnumMember,
                        arrow, `Arrow type: ${arrow}`);
                });
                return { suggestions };
            }

            if (isDirContext) {
                ['forward', 'back', 'both', 'none'].forEach(dir => {
                    addSuggestion(dir, monaco.languages.CompletionItemKind.EnumMember,
                        dir, `Edge direction: ${dir}`);
                });
                return { suggestions };
            }

            if (isRankdirContext) {
                [['TB', 'Top to Bottom'], ['BT', 'Bottom to Top'], 
                 ['LR', 'Left to Right'], ['RL', 'Right to Left']].forEach(([val, desc]) => {
                    addSuggestion(val, monaco.languages.CompletionItemKind.EnumMember,
                        val, `Rank direction: ${desc}`);
                });
                return { suggestions };
            }

            if (isSplinesContext) {
                ['ortho', 'spline', 'polyline', 'line', 'curved', 'none', 'true', 'false'].forEach(val => {
                    addSuggestion(val, monaco.languages.CompletionItemKind.EnumMember,
                        val, `Splines mode: ${val}`);
                });
                return { suggestions };
            }

            // Image attribute - suggest shape library icons
            const isImageContext = /image\s*=\s*"?$/.test(textBefore);
            if (isImageContext) {
                // AWS icons
                const awsIcons = [
                    ['shapes/aws/s3.svg', 'S3 Bucket', 'AWS storage service'],
                    ['shapes/aws/ec2.svg', 'EC2 Instance', 'AWS compute service'],
                    ['shapes/aws/lambda.svg', 'Lambda Function', 'AWS serverless compute'],
                    ['shapes/aws/rds.svg', 'RDS Database', 'AWS relational database'],
                    ['shapes/aws/sqs.svg', 'SQS Queue', 'AWS message queue'],
                    ['shapes/aws/dynamodb.svg', 'DynamoDB', 'AWS NoSQL database'],
                    ['shapes/aws/ecs.svg', 'ECS Container', 'AWS container service'],
                    ['shapes/aws/api-gateway.svg', 'API Gateway', 'AWS API management']
                ];
                awsIcons.forEach(([path, label, desc]) => {
                    addSuggestion(path, monaco.languages.CompletionItemKind.File,
                        path, `AWS: ${label}`, false, desc);
                });

                // Kubernetes icons
                const k8sIcons = [
                    ['shapes/kubernetes/pod.svg', 'Pod', 'K8s smallest deployable unit'],
                    ['shapes/kubernetes/service.svg', 'Service', 'K8s network abstraction'],
                    ['shapes/kubernetes/deployment.svg', 'Deployment', 'K8s workload controller'],
                    ['shapes/kubernetes/node.svg', 'Node', 'K8s worker machine'],
                    ['shapes/kubernetes/ingress.svg', 'Ingress', 'K8s external access'],
                    ['shapes/kubernetes/configmap.svg', 'ConfigMap', 'K8s configuration'],
                    ['shapes/kubernetes/pv.svg', 'PersistentVolume', 'K8s storage'],
                    ['shapes/kubernetes/job.svg', 'Job', 'K8s batch workload']
                ];
                k8sIcons.forEach(([path, label, desc]) => {
                    addSuggestion(path, monaco.languages.CompletionItemKind.File,
                        path, `K8s: ${label}`, false, desc);
                });

                // Infrastructure icons
                const infraIcons = [
                    ['shapes/infrastructure/server.svg', 'Server', 'Generic server'],
                    ['shapes/infrastructure/database.svg', 'Database', 'Generic database'],
                    ['shapes/infrastructure/computer.svg', 'Computer', 'Desktop/workstation'],
                    ['shapes/infrastructure/network.svg', 'Network', 'Network topology'],
                    ['shapes/infrastructure/cloud.svg', 'Cloud', 'Cloud provider'],
                    ['shapes/infrastructure/storage.svg', 'Storage', 'Storage array'],
                    ['shapes/infrastructure/firewall.svg', 'Firewall', 'Security firewall'],
                    ['shapes/infrastructure/load-balancer.svg', 'Load Balancer', 'Traffic distribution']
                ];
                infraIcons.forEach(([path, label, desc]) => {
                    addSuggestion(path, monaco.languages.CompletionItemKind.File,
                        path, `Infra: ${label}`, false, desc);
                });

                // Data/Pipeline icons
                const dataIcons = [
                    ['shapes/data/postgres.svg', 'PostgreSQL', 'SQL database'],
                    ['shapes/data/duckdb.svg', 'DuckDB', 'Analytics database'],
                    ['shapes/data/parquet.svg', 'Parquet', 'Columnar data format'],
                    ['shapes/data/kafka.svg', 'Apache Kafka', 'Event streaming'],
                    ['shapes/data/spark.svg', 'Apache Spark', 'Big data processing'],
                    ['shapes/data/airflow.svg', 'Apache Airflow', 'Workflow orchestration'],
                    ['shapes/data/redis.svg', 'Redis', 'In-memory cache']
                ];
                dataIcons.forEach(([path, label, desc]) => {
                    addSuggestion(path, monaco.languages.CompletionItemKind.File,
                        path, `Data: ${label}`, false, desc);
                });

                // Software icons
                const softwareIcons = [
                    ['shapes/software/rust.svg', 'Rust', 'Systems programming'],
                    ['shapes/software/python.svg', 'Python', 'Scripting language'],
                    ['shapes/software/docker.svg', 'Docker', 'Container runtime'],
                    ['shapes/software/api.svg', 'API Service', 'REST/gRPC endpoint'],
                    ['shapes/software/react.svg', 'React', 'Frontend framework'],
                    ['shapes/software/tauri.svg', 'Tauri', 'Desktop app framework'],
                    ['shapes/software/qt.svg', 'Qt', 'Desktop app framework'],
                    ['shapes/software/console.svg', 'Console App', 'CLI application']
                ];
                softwareIcons.forEach(([path, label, desc]) => {
                    addSuggestion(path, monaco.languages.CompletionItemKind.File,
                        path, `Software: ${label}`, false, desc);
                });

                return { suggestions };
            }

            // === Attribute name completions ===
            if (isInAttributes && !isAfterEquals) {
                // Common attributes
                const commonAttrs = [
                    ['shape', 'Node shape (box, ellipse, cylinder, etc.)'],
                    ['label', 'Text label for node/edge'],
                    ['color', 'Outline color'],
                    ['fillcolor', 'Fill color (requires style=filled)'],
                    ['style', 'Visual style (solid, dashed, filled, etc.)'],
                    ['fontname', 'Font family name'],
                    ['fontsize', 'Font size in points'],
                    ['fontcolor', 'Font color'],
                    ['penwidth', 'Line thickness'],
                    ['width', 'Node width in inches'],
                    ['height', 'Node height in inches'],
                    ['fixedsize', 'Use exact width/height'],
                    ['tooltip', 'Tooltip text'],
                    ['href', 'URL link'],
                    ['image', 'Background image path'],
                    ['arrowhead', 'Arrow style at head'],
                    ['arrowtail', 'Arrow style at tail'],
                    ['dir', 'Edge direction (forward, back, both, none)'],
                    ['constraint', 'Edge affects ranking'],
                    ['weight', 'Edge weight for layout'],
                    ['minlen', 'Minimum edge length']
                ];

                commonAttrs.forEach(([attr, desc]) => {
                    addSuggestion(attr, monaco.languages.CompletionItemKind.Property,
                        attr + '=${1}', desc, true);
                });
                return { suggestions };
            }

            // === Top-level completions ===
            
            // Digraph template
            addSuggestion('digraph', monaco.languages.CompletionItemKind.Keyword,
                'digraph ${1:G} {\n\t// Graph attributes\n\trankdir=${2|TB,LR,BT,RL|};\n\tsplines=${3|ortho,spline,polyline|};\n\t\n\t// Nodes\n\t${4:node1} [label="${5:Node 1}"];\n\t${6:node2} [label="${7:Node 2}"];\n\t\n\t// Edges\n\t${4:node1} -> ${6:node2};\n\t$0\n}',
                'Directed graph', true,
                'Creates a new directed graph with common settings');

            // Graph template
            addSuggestion('graph', monaco.languages.CompletionItemKind.Keyword,
                'graph ${1:G} {\n\t// Graph attributes\n\tlayout=${2|neato,fdp,sfdp,circo|};\n\t\n\t// Nodes\n\t${3:node1} [label="${4:Node 1}"];\n\t${5:node2} [label="${6:Node 2}"];\n\t\n\t// Edges\n\t${3:node1} -- ${5:node2};\n\t$0\n}',
                'Undirected graph', true,
                'Creates a new undirected graph');

            // Subgraph/cluster template
            addSuggestion('subgraph cluster', monaco.languages.CompletionItemKind.Keyword,
                'subgraph cluster_${1:name} {\n\tlabel="${2:Cluster}";\n\tstyle=filled;\n\tfillcolor="#f0f0f0";\n\t\n\t$0\n}',
                'Cluster subgraph', true,
                'Creates a visual cluster (name must start with "cluster_")');

            // Node defaults
            addSuggestion('node defaults', monaco.languages.CompletionItemKind.Snippet,
                'node [\n\tshape=${1|box,ellipse,cylinder,rectangle,diamond|};\n\tstyle="${2|filled,rounded,filled,rounded|}";\n\tfillcolor="${3:#e0e0e0}";\n\tfontname="${4:Helvetica}";\n\tfontsize=${5:12};\n];',
                'Default node attributes', true);

            // Edge defaults
            addSuggestion('edge defaults', monaco.languages.CompletionItemKind.Snippet,
                'edge [\n\tcolor="${1:#666666}";\n\tpenwidth=${2:1.5};\n\tarrowhead=${3|normal,vee,dot,none|};\n\tfontname="${4:Helvetica}";\n\tfontsize=${5:10};\n];',
                'Default edge attributes', true);

            // Kubernetes cluster example
            addSuggestion('kubernetes cluster', monaco.languages.CompletionItemKind.Snippet,
                'subgraph cluster_k8s {\n\tlabel="Kubernetes Cluster";\n\tstyle=filled;\n\tfillcolor="#326CE5";\n\tfontcolor=white;\n\t\n\tmaster [label="Control Plane", shape=box3d, fillcolor="#1E4D8C", fontcolor=white, style=filled];\n\t\n\tsubgraph cluster_workers {\n\t\tlabel="Worker Nodes";\n\t\tfillcolor="#4A90D9";\n\t\t\n\t\tworker1 [label="Worker 1", shape=box, fillcolor="#6BA8E5", style=filled];\n\t\tworker2 [label="Worker 2", shape=box, fillcolor="#6BA8E5", style=filled];\n\t}\n\t\n\tmaster -> worker1;\n\tmaster -> worker2;\n}',
                'K8s cluster template', true);

            // Database connection example
            addSuggestion('database connection', monaco.languages.CompletionItemKind.Snippet,
                '${1:app} [label="${2:Application}", shape=component, style=filled, fillcolor="#90CAF9"];\n${3:db} [label="${4:Database}", shape=cylinder, style=filled, fillcolor="#FFF59D"];\n${1:app} -> ${3:db} [label="${5:SQL}", style=dashed];',
                'App-to-database connection', true);

            // Ranks (same rank)
            addSuggestion('same rank', monaco.languages.CompletionItemKind.Snippet,
                '{ rank=same; ${1:node1}; ${2:node2}; }',
                'Force nodes to same rank', true);

            // Colors
            addSuggestion('color palette', monaco.languages.CompletionItemKind.Snippet,
                '// Blue tones\n// #E3F2FD #BBDEFB #90CAF9 #64B5F6 #42A5F5 #2196F3 #1E88E5 #1976D2\n// Green tones\n// #E8F5E9 #C8E6C9 #A5D6A7 #81C784 #66BB6A #4CAF50 #43A047 #388E3C\n// Orange tones\n// #FFF3E0 #FFE0B2 #FFCC80 #FFB74D #FFA726 #FF9800 #FB8C00 #F57C00',
                'Material Design color palette', true);

            // Node with image icon
            addSuggestion('node with icon', monaco.languages.CompletionItemKind.Snippet,
                '${1:node_id} [label="${2:Label}", shape=none, image="${3|shapes/aws/ec2.svg,shapes/aws/s3.svg,shapes/aws/lambda.svg,shapes/data/postgres.svg,shapes/data/duckdb.svg,shapes/software/rust.svg,shapes/kubernetes/pod.svg|}", labelloc=b];',
                'Node with shape library icon', true);

            // Data pipeline template
            addSuggestion('data pipeline', monaco.languages.CompletionItemKind.Snippet,
                'subgraph cluster_pipeline {\n\tlabel="Data Pipeline";\n\tstyle=filled;\n\tfillcolor="#E3F2FD";\n\t\n\tsource [label="Source", shape=none, image="shapes/data/postgres.svg", labelloc=b];\n\tprocess [label="Process", shape=none, image="shapes/data/spark.svg", labelloc=b];\n\tsink [label="Sink", shape=none, image="shapes/data/duckdb.svg", labelloc=b];\n\t\n\tsource -> process -> sink;\n}',
                'Data pipeline with icons', true);

            // Rust backtester template
            addSuggestion('rust backtester', monaco.languages.CompletionItemKind.Snippet,
                'subgraph cluster_backtester {\n\tlabel="Rust Backtest Engine";\n\tstyle=filled;\n\tfillcolor="#FFF3E0";\n\t\n\tengine [label="Backtest Engine", shape=none, image="shapes/software/rust.svg", labelloc=b];\n\tindicators [label="Indicators", shape=component, style=filled, fillcolor="#FFE0B2"];\n\tsweep [label="Parameter Sweep", shape=component, style=filled, fillcolor="#FFE0B2"];\n\twfo [label="Walk-Forward", shape=component, style=filled, fillcolor="#FFE0B2"];\n\t\n\tengine -> indicators;\n\tengine -> sweep;\n\tsweep -> wfo;\n}',
                'Rust backtest engine template', true);

            return { suggestions: suggestions };
        }
    });
}

/**
 * Define the Strategient dark theme for Monaco with DOT-specific colors
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
            { token: 'string.html', foreground: 'D19A66' },
            { token: 'string.html.tag', foreground: 'E06C75' },
            
            // Keywords - blue for graph types
            { token: 'keyword', foreground: '569CD6' },
            { token: 'keyword.graph', foreground: 'C586C0', fontStyle: 'bold' },
            { token: 'keyword.modifier', foreground: '569CD6' },
            { token: 'keyword.attribute', foreground: '4EC9B0' },
            
            // Attributes - cyan/teal
            { token: 'attribute.name', foreground: '9CDCFE' },
            { token: 'attribute.value', foreground: 'CE9178' },
            
            // Operators - light gray
            { token: 'operator', foreground: 'D4D4D4' },
            { token: 'operator.edge', foreground: '61AFEF', fontStyle: 'bold' },
            
            // Identifiers - default text / light purple for nodes
            { token: 'identifier', foreground: 'ABB2BF' },
            
            // Numbers - light green
            { token: 'number', foreground: 'B5CEA8' },
            
            // Colors - gold
            { token: 'constant.color', foreground: 'DCDCAA' },
            
            // Delimiters
            { token: 'delimiter', foreground: 'D4D4D4' },
            { token: '@brackets', foreground: 'FFD700' }
        ],
        colors: {
            'editor.background': '#1E1E1E',
            'editor.foreground': '#D4D4D4',
            'editorLineNumber.foreground': '#5A5A5A',
            'editorLineNumber.activeForeground': '#C6C6C6',
            'editorGutter.background': '#1E1E1E',
            'editor.selectionBackground': '#264F78',
            'editor.selectionHighlightBackground': '#3A3D41',
            'editor.inactiveSelectionBackground': '#3A3D41',
            'editor.lineHighlightBackground': '#2A2D2E',
            'editor.lineHighlightBorder': '#2A2D2E',
            'editorCursor.foreground': '#AEAFAD',
            'editorWhitespace.foreground': '#3B3B3B',
            'editorIndentGuide.background': '#404040',
            'editorIndentGuide.activeBackground': '#707070',
            'scrollbarSlider.background': '#4E4E4E80',
            'scrollbarSlider.hoverBackground': '#646464B0',
            'scrollbarSlider.activeBackground': '#6E6E6EE0',
            'editorWidget.background': '#252526',
            'editorWidget.border': '#454545',
            'editorSuggestWidget.background': '#252526',
            'editorSuggestWidget.border': '#454545',
            'editorSuggestWidget.selectedBackground': '#094771',
            'editorSuggestWidget.highlightForeground': '#0097FB',
            'editor.findMatchBackground': '#515C6A',
            'editor.findMatchHighlightBackground': '#EA5C0055',
            'minimap.background': '#1E1E1E',
            'minimapSlider.background': '#4E4E4E40',
            'editorBracketMatch.background': '#0064001A',
            'editorBracketMatch.border': '#888888'
        }
    });
}

