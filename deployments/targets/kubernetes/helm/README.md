# NOS3 Helm Chart for Kubernetes

Parameterized Helm chart that deploys any combination of NOS3 services to Kubernetes.
Designed for Nutanix Kubernetes Platform (NKE/NKP) with support for Docker Desktop K8s testing.

## Chart Structure

```
helm/nos3/
  Chart.yaml                        # Chart metadata (v0.1.0)
  values.yaml                       # Full service registry — all 25 services
  values-minimum.yaml               # Profile: core + essential sims only
  values-nkp.yaml                   # Profile: Nutanix NKP overrides
  configs/                          # Optional config files (mount at install time)
  NOTES.txt                         # Post-install instructions
  templates/
    _helpers.tpl                    # Shared named templates (fullname, labels, env)
    namespace.yaml                  # Namespace with PSS labels (opt-in)
    configmap-entrypoints.yaml      # Entrypoint scripts for core services
    configmap-configs.yaml          # nos3-simulator.xml, nos_engine_server_config.json
    deployment-loop.yaml            # Single template → all 25 Deployments
    service-loop.yaml               # Single template → all 25 Services
    alias-services.yaml             # DNS alias Services (Compose network aliases)
    networkpolicy.yaml              # Compose-style network segmentation (opt-in)
    ingress.yaml                    # External access via Ingress (opt-in)
    serviceaccount.yaml             # RBAC ServiceAccount
```

## Architecture Diagram

```
┌──────────────────────────────────────────────────────────────────────────────────┐
│  Kubernetes Cluster                                                              │
│                                                                                  │
│  ┌────────────────────────────────────────────────────────────────────────────┐  │
│  │  Namespace: {project}-{mission}   (e.g., ssmo-mms)                        │  │
│  │                                                                            │  │
│  │  ┌──────────────────────────────────────────────────────────────────────┐  │  │
│  │  │  Core Services                                                      │  │  │
│  │  │                                                                      │  │  │
│  │  │  ┌──────────┐ ┌─────────────────┐ ┌──────┐ ┌──────┐ ┌────────┐    │  │  │
│  │  │  │ FortyTwo  │ │nos-engine-server│ │ FSW  │ │ GSW  │ │ OpenMCT│    │  │  │
│  │  │  │ (42 sim)  │ │   (NOS bus)     │ │(cFS) │ │(YAMCS│ │  (web) │    │  │  │
│  │  │  │  :80/vnc  │ │ :12000,:12001   │ │      │ │:8090)│ │  :9000 │    │  │  │
│  │  │  └──────────┘ └─────────────────┘ └──────┘ └──────┘ └────────┘    │  │  │
│  │  └──────────────────────────────────────────────────────────────────────┘  │  │
│  │                                                                            │  │
│  │  ┌──────────────────────────────────────────────────────────────────────┐  │  │
│  │  │  Simulators (15 pods)                                                │  │  │
│  │  │                                                                      │  │  │
│  │  │  truth42sim  camsim  css-sim  eps-sim  fss-sim  gps-sim  imu-sim    │  │  │
│  │  │  mag-sim  rw-sim0  rw-sim1  rw-sim2  radio-sim-cryptolib           │  │  │
│  │  │  sample-sim  startrk-sim  thruster-sim  torquer-sim                 │  │  │
│  │  │                                                                      │  │  │
│  │  │  All run: ./nos3-single-simulator -f ./nos3-simulator.xml <name>    │  │  │
│  │  └──────────────────────────────────────────────────────────────────────┘  │  │
│  │                                                                            │  │
│  │  ┌──────────────────────────────────────────────────────────────────────┐  │  │
│  │  │  Terminals / Utilities (4 pods)                                      │  │  │
│  │  │                                                                      │  │  │
│  │  │  nos-terminal   nos-udp-terminal   nos-sim-bridge   time            │  │  │
│  │  └──────────────────────────────────────────────────────────────────────┘  │  │
│  │                                                                            │  │
│  │  ┌──────────────────────────────────────────────────────────────────────┐  │  │
│  │  │  Supporting Resources                                                │  │  │
│  │  │                                                                      │  │  │
│  │  │  ConfigMap: entrypoints        (5 entrypoint scripts)               │  │  │
│  │  │  ConfigMap: sim-config         (nos3-simulator.xml)                  │  │  │
│  │  │  ConfigMap: nos-engine-config  (nos_engine_server_config.json)       │  │  │
│  │  │  ConfigMap: gsw-config         (settings.xml)                        │  │  │
│  │  │  ConfigMap: openmct-config     (webpack, example files)              │  │  │
│  │  │  ConfigMap: fortytwo-inout     (InOut/*.txt — pre-created)           │  │  │
│  │  │  ServiceAccount               (RBAC)                                │  │  │
│  │  │  Services: 25 primary + 33 DNS aliases = 58 total                   │  │  │
│  │  │  NetworkPolicies (opt-in): default-deny, mission, fleet             │  │  │
│  │  └──────────────────────────────────────────────────────────────────────┘  │  │
│  └────────────────────────────────────────────────────────────────────────────┘  │
│                                                                                  │
│  ┌──────────────────────────┐                                                    │
│  │  Ingress (opt-in)        │  {project}.{baseDomain}         → FortyTwo :80     │
│  │  (NKP / nginx)           │  yamcs.{project}.{baseDomain}   → GSW      :8090   │
│  │                          │  openmct.{project}.{baseDomain} → OpenMCT  :9000   │
│  └──────────────────────────┘                                                    │
└──────────────────────────────────────────────────────────────────────────────────┘
```

## Startup Order (initContainers)

```
                          ┌────────────┐
                          │  FortyTwo   │  ← starts first (no dependencies)
                          └──────┬─────┘
                                 │ :80
                          ┌──────▼─────┐
                 ┌────────│nos-engine  │────────┐
                 │        │  -server   │        │
                 │        └──────┬─────┘        │
                 │               │ :12000       │
           ┌─────▼───┐   ┌──────▼─────┐   ┌────▼────────┐
           │   FSW    │   │    GSW     │   │ Simulators  │
           │  (cFS)   │   │  (YAMCS)   │   │ (15 pods)   │
           └─────┬────┘   └──────┬─────┘   └─────────────┘
                 │               │ :8090
                 │ :60000 ┌──────▼─────┐
                 │        │  OpenMCT   │
                 │        └────────────┘
                 │        ┌────────────┐
                 └───────►│radio-sim   │
                          │-cryptolib  │
                          └────────────┘
```

Services use initContainers with `nc -z` (TCP) or `wget --spider` (HTTP) to wait for
dependencies before starting, matching Docker Compose `depends_on: condition: service_healthy`.

## Network Segmentation (NetworkPolicies)

When `networkPolicy.enabled: true`, three policy layers enforce the Docker Compose
three-network model:

```
┌────────────────────────────────────────────────────────────────────────┐
│                                                                        │
│  Fleet Network (nos3.nasa.gov/project label)                          │
│  ┌──────────────────────────────────────────────────────────────────┐  │
│  │                                                                  │  │
│  │  Mission Network (nos3.nasa.gov/mission label)                  │  │
│  │  ┌────────────────────────────────────────────────────────────┐  │  │
│  │  │                                                            │  │  │
│  │  │  Spacecraft Namespace (same namespace)                     │  │  │
│  │  │                                                            │  │  │
│  │  │  All 25 pods can freely communicate                        │  │  │
│  │  │  (mission-spacecraft-network equivalent)                   │  │  │
│  │  │                                                            │  │  │
│  │  └────────────────────────────────────────────────────────────┘  │  │
│  │                                                                  │  │
│  │  Other spacecraft namespaces (same mission) can reach all pods  │  │
│  │  (mission-network equivalent)                                   │  │
│  │                                                                  │  │
│  └──────────────────────────────────────────────────────────────────┘  │
│                                                                        │
│  Only OpenMCT is accessible fleet-wide (fleet-network equivalent)     │
│                                                                        │
└────────────────────────────────────────────────────────────────────────┘
```

| K8s NetworkPolicy | Compose Network Equivalent | Scope |
|---|---|---|
| `default-deny` + intra-ns allow | `mission-spacecraft-network` | All pods within one spacecraft namespace |
| `mission-network` | `mission-network` | Cross-namespace traffic with same `nos3.nasa.gov/mission` label |
| `openmct-fleet` | `fleet-network` | OpenMCT reachable from any namespace with same `nos3.nasa.gov/project` label |

## How Docker Compose Maps to Kubernetes

This Helm chart was derived directly from `deployments/compose.yaml`. Each Compose
service definition was translated to a data entry in `values.yaml` using the following
mapping:

### Service Definition

| Docker Compose | Helm values.yaml | K8s Resource |
|---|---|---|
| Service name | `services.<name>` key | Deployment + Service name |
| `image:` | `services.<name>.image.repository` / `.tag` | `container.image` |
| `command:` | `services.<name>.command` | `container.command` |
| `working_dir:` | `services.<name>.workingDir` | `container.workingDir` |
| `environment:` | `services.<name>.env` | `container.env` |
| `ports:` | `services.<name>.ports` | Service `spec.ports` |

### Health & Dependencies

| Docker Compose | Helm values.yaml | K8s Resource |
|---|---|---|
| `healthcheck: test:` | `services.<name>.healthCheck.exec` | `readinessProbe` + `livenessProbe` |
| `healthcheck: test: ["CMD", "wget"...]` | `services.<name>.healthCheck.httpGet` | `httpGet` probe |
| `depends_on: condition: service_healthy` | `services.<name>.waitFor` | `initContainers` with `nc -z` / `wget` |

### Security & Privileges

| Docker Compose | Helm values.yaml | K8s Resource |
|---|---|---|
| `privileged: true` | `services.<name>.securityContext.privileged` | `container.securityContext.privileged` |
| `cap_add: [SYS_NICE]` | `services.<name>.securityContext.capabilities.add` | `container.securityContext.capabilities.add` |
| `sysctls: fs.mqueue.msg_max` | `services.<name>.sysctls` | `pod.spec.securityContext.sysctls` |

### Networking

| Docker Compose | Helm values.yaml | K8s Resource |
|---|---|---|
| `networks: aliases: [name]` | `services.<name>.dnsAliases` | Additional headless Services (`alias-services.yaml`) |
| Three named networks | `networkPolicy.enabled` | NetworkPolicies (default-deny, mission, fleet) |
| Service DNS (by service name) | Automatic | Service DNS: `<name>.<namespace>.svc.cluster.local` |

### Volumes

| Docker Compose | Helm values.yaml | K8s Resource |
|---|---|---|
| `volumes: - ./entrypoint.sh:/entrypoint.sh` | `services.<name>.entrypointKey` | ConfigMap mount from `configmap-entrypoints` |
| `volumes: - ./nos3-simulator.xml:...` | `services.<name>.simName` (auto-mounts) | ConfigMap mount from `configmap-configs` |
| `volumes: - ./nos_engine_server_config.json:...` | `services.<name>.nosEngineConfig: true` | ConfigMap mount from `configmap-configs` |

### Simulator Pattern

In Docker Compose, all 15 simulators share the same image and run:
```
./nos3-single-simulator -f ./nos3-simulator.xml <simulator-name>
```

In the Helm chart, this is the `simName` pattern. Setting `simName` on a service entry
automatically:
1. Sets the command to `["./nos3-single-simulator", "-f", "./nos3-simulator.xml", "<simName>"]`
2. Mounts `nos3-simulator.xml` from the `sim-config` ConfigMap
3. No per-simulator template code needed

## Data-Driven Design: Adding/Removing Services

The chart uses **two loop templates** (`deployment-loop.yaml`, `service-loop.yaml`) to
generate all resources. No per-service template files exist. This makes the chart fully
data-driven:

| Operation | What to change | Templates touched |
|---|---|---|
| Add a simulator | Add entry to `services:` with `simName` | None |
| Add a core service | Add entry with `command`, `image`, `ports` | None |
| Remove a service | Set `enabled: false` or delete entry | None |
| Change image/resources | Edit the service entry | None |
| Add entrypoint script | Add key to `configmap-entrypoints.yaml`, set `entrypointKey` | 1 file |
| Change startup order | Edit `waitFor` array | None |
| Override for specific deploy | Create `values-<env>.yaml` with overrides | None (new file) |

### Example: Adding a New Simulator

```yaml
# In values.yaml, under services:
  my-new-sim:
    enabled: true
    group: simulator
    image: *simImage
    simName: my-new-sim           # ← this is all you need for the sim pattern
    workingDir: *simWorkDir
    resources: *simResources
    securityContext: *privileged
    dnsAliases: [my-new-sim]
    waitFor: *simWaitFor
```

No template changes. `helm upgrade` will create the Deployment, Service, and alias Service.

## Quick Start

### Single Spacecraft (build + deploy)

```bash
# Build images and deploy (all-in-one)
task k8s:helm:up PROJECT=ssmo MISSION=mms SPACECRAFT=mms1 \
  FORTYTWO_HOST_PORT=30091 YAMCS_HOST_PORT=8091 OPENMCT_HOST_PORT=9001

# Minimum profile (core + essential sims)
task k8s:helm:install:minimum

# Uninstall
task k8s:helm:delete PROJECT=ssmo MISSION=mms SPACECRAFT=mms1
```

### Multiple Spacecraft in Parallel

```bash
# Step 1: Build images once
task k8s:helm:build

# Step 2: Deploy spacecraft in parallel (each gets unique host ports)
task k8s:helm:install PROJECT=ssmo MISSION=mms SPACECRAFT=mms1 \
  FORTYTWO_HOST_PORT=30091 YAMCS_HOST_PORT=8091 OPENMCT_HOST_PORT=9001 &
task k8s:helm:install PROJECT=ssmo MISSION=mms SPACECRAFT=mms2 \
  FORTYTWO_HOST_PORT=30092 YAMCS_HOST_PORT=8092 OPENMCT_HOST_PORT=9002 &
wait

# Tear down all
task k8s:helm:delete PROJECT=ssmo MISSION=mms SPACECRAFT=mms1 &
task k8s:helm:delete PROJECT=ssmo MISSION=mms SPACECRAFT=mms2 &
wait
```

### Nutanix NKP

```bash
# With NKP overrides (ingress, storage class, network policies)
task k8s:helm:install:nkp

# Custom mission/spacecraft
task k8s:helm:install PROJECT=ssmo MISSION=mms SPACECRAFT=mms3
```

### Manual Helm Commands

```bash
# Render templates locally (no cluster needed)
helm template nos3 ./deployments/targets/kubernetes/helm/nos3

# Install with real config files
helm upgrade --install nos3-m01-sc01 ./deployments/targets/kubernetes/helm/nos3 \
  --namespace nos3-m01 --create-namespace \
  --set-file simulatorConfig=./cfg/build/sims/nos3-simulator.xml \
  --set-file nosEngineConfig=./cfg/build/sims/nos_engine_server_config.json

# Override specific service
helm upgrade nos3-m01-sc01 ./deployments/targets/kubernetes/helm/nos3 \
  --set services.camsim.enabled=false \
  --set services.fortytwo.resources.limits.cpu=4
```

## Value Profiles

| Profile | File | Services | Use Case |
|---|---|---|---|
| Full | `values.yaml` (default) | All 25 | Complete simulation environment |
| Minimum | `values-minimum.yaml` | 9 | Quick testing, reduced resources |
| NKP | `values-nkp.yaml` | All 25 | Nutanix cluster with ingress & network policies |

## Prerequisites

### Docker Desktop
- Kubernetes enabled in Docker Desktop settings
- Helm v3 installed
- NOS3 images built locally (`task build`)

### Nutanix NKP
- Kubelet configured with `--allowed-unsafe-sysctls=fs.mqueue.*` (FSW requirement)
- Nutanix CSI driver installed (for PVC storage)
- Ingress controller available (nginx)
- All NOS3 images pushed to a reachable registry
- Namespace labeled with `pod-security.kubernetes.io/enforce: privileged`
