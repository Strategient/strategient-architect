Strategient-Architect Implementation - Session 6

<title>
Infrastructure Config page: provider abstraction + mock provider + editable service URLs + optional embedded Web UIs

<description>
Implement the first “operationally useful” page: Infrastructure Config. End state: the page shows machines and services with statuses; selecting a machine/service shows details in an inspector; service URLs are editable and persist; clicking “Open Web UI” opens either an embedded WebView tab (if enabled) or external browser.

Cursor Prompt to initiate session 6 build (mostly autonomous with some human assistance where needed):
<Prompt>
You are working inside `~/projects/strategient-architect`.

Goal:
- Add `InfraConfig` page type with a strict provider interface (plugin-ready).
- Implement `MockInfraProvider` that supplies sample machines/services/statuses.
- Persist infra config edits in page metadata keyed by PlantUML node IDs.
- Provide quick access to service Web UIs (Airflow/Spark/Kafka/K8s dashboards).

Hard constraints:
- Do NOT hard-wire “Apache forever”. Infra providers must be pluggable.
- Must work on bare metal AND containerized environments:
  - “provider” could be Local mock, SSH, Docker, Kubernetes, or Cloud API.
- UI must not assume QtWebEngine is available:
  - If WebEngine is enabled, embed.
  - Otherwise open external browser.

IR requirements:
PlantUML + metadata requirements:
- The InfraConfig diagram structure is canonical PlantUML in `page.plantuml`.
- Each clickable machine/service MUST be declared with `as <ID>`.
- Store infra-specific data in `page.metadata.nodes[ID]`, e.g.:
  - machine fields:
    - `display_name`
    - `ssh_host`, `ssh_user`, `tags[]` (optional)
  - service fields:
    - `web_url` (optional)
    - `provider_key` (optional; which provider to query)
  - runtime status fields (written by provider/controller):
    - `status`, `last_seen`, `version` (optional)

Architecture:
- Add `core/infra/IInfraProvider` abstract base class:
  - `validate()`, `fetchSnapshot()`, `startService()`, `stopService()`
- Add `core/infra/MockInfraProvider` implementing the interface.
- Add `core/infra/InfraService` that owns a provider instance and is used by the Infra page controller.

UI:
- On the canvas: show each machine/service node from parsed PlantUML as a node box.
- Clicking a node shows its metadata and provider-driven status in the inspector.
- Inspector:
  - machine list, service list, selected service details
  - edit `web_url` and persist into IR
  - buttons: Refresh Status, Open Web UI

Acceptance:
- User can edit a service `web_url`, save project, reload, and it persists.
- Clicking Open Web UI opens the configured URL (embedded if available; external otherwise).
- Mock provider returns at least 4 machines (Aurora, HP Envy, Titan, EC2-1) with a few services each.
</Prompt>

Proposed Implmentation of Session 6:
- Introduce provider abstraction (future-proof):
  - `MockInfraProvider` first
  - Later providers may include:
    - `SshInfraProvider` (bare metal)
    - `DockerInfraProvider` (containers on a host)
    - `KubernetesInfraProvider` (calls `kubectl` to list pods/services)
    - Cloud provider (AWS)
- Keep initial UI “boring”: machine nodes + inspector-based editing + quick links.

Testable Artifacts and Excpected Functions:
- InfraConfig page renders machine nodes.
- Inspector edits persist in `.sarch.json`.
- Web UI open works in at least one mode.


