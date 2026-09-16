{{/*
NOS3 Helm Chart - Shared Named Templates
*/}}

{{/*
Generate the full resource name: {project}-{mission}-{spacecraft}-{component}
Usage: {{ include "nos3.fullname" (dict "root" . "component" "fortytwo") }}
*/}}
{{- define "nos3.fullname" -}}
{{ .root.Values.global.project }}-{{ .root.Values.global.mission }}-{{ .root.Values.global.spacecraft }}-{{ .component }}
{{- end -}}

{{/*
Generate the chart-level fullname (no component)
Usage: {{ include "nos3.releaseName" . }}
*/}}
{{- define "nos3.releaseName" -}}
{{ .Values.global.project }}-{{ .Values.global.mission }}-{{ .Values.global.spacecraft }}
{{- end -}}

{{/*
Generate the namespace: {project}-{mission}-{spacecraft}
Each spacecraft gets its own namespace (maps to Compose's mission-spacecraft-network).
Cross-spacecraft communication uses mission-network NetworkPolicy (same mission label).
Usage: {{ include "nos3.namespace" . }}
*/}}
{{- define "nos3.namespace" -}}
{{ .Values.global.namespaceOverride | default (printf "%s-%s-%s" .Values.global.project .Values.global.mission .Values.global.spacecraft) }}
{{- end -}}

{{/*
Common labels for a component
Usage: {{ include "nos3.labels" (dict "root" . "component" "fortytwo") | nindent 4 }}
*/}}
{{- define "nos3.labels" -}}
app.kubernetes.io/name: {{ .component }}
app.kubernetes.io/instance: {{ .root.Release.Name }}
app.kubernetes.io/part-of: nos3
app.kubernetes.io/managed-by: {{ .root.Release.Service }}
nos3.nasa.gov/project: {{ .root.Values.global.project }}
nos3.nasa.gov/mission: {{ .root.Values.global.mission }}
nos3.nasa.gov/spacecraft: {{ .root.Values.global.spacecraft }}
nos3.nasa.gov/component: {{ .component }}
{{- end -}}

{{/*
Selector labels (subset used in spec.selector.matchLabels)
Usage: {{ include "nos3.selectorLabels" (dict "root" . "component" "fortytwo") | nindent 6 }}
*/}}
{{- define "nos3.selectorLabels" -}}
app.kubernetes.io/name: {{ .component }}
app.kubernetes.io/instance: {{ .root.Release.Name }}
{{- end -}}

{{/*
Common environment variables injected into ALL pods
Usage: {{ include "nos3.commonEnv" .root | nindent 12 }}
*/}}
{{- define "nos3.commonEnv" -}}
- name: MISSION
  value: {{ .Values.global.mission | quote }}
- name: SPACECRAFT
  value: {{ .Values.global.spacecraft | quote }}
- name: PROJECT_NAME
  value: "{{ .Values.global.project }}-{{ .Values.global.mission }}-{{ .Values.global.spacecraft }}"
{{- if .Values.global.env }}
{{- range $key, $val := .Values.global.env }}
{{- if $val }}
- name: {{ $key }}
  value: {{ $val | quote }}
{{- end }}
{{- end }}
{{- end }}
{{- end -}}

{{/*
Simulator config volume mount (nos3-simulator.xml)
Usage: {{ include "nos3.simConfigVolumeMount" .root | nindent 12 }}
*/}}
{{- define "nos3.simConfigVolumeMount" -}}
- name: sim-config
  mountPath: /home/nos3/builds/nos3/sims/build/bin/nos3-simulator.xml
  subPath: nos3-simulator.xml
{{- end -}}

{{/*
Simulator config volume definition
Usage: {{ include "nos3.simConfigVolume" (dict "root" . "component" "sim-config") | nindent 8 }}
*/}}
{{- define "nos3.simConfigVolume" -}}
- name: sim-config
  configMap:
    name: {{ include "nos3.fullname" (dict "root" .root "component" "sim-config") }}
{{- end -}}

{{/*
Entrypoint volume mount from ConfigMap
Usage: {{ include "nos3.entrypointVolumeMount" (dict "entrypointKey" "entrypoint-fsw.sh" "mountPath" "/entrypoint.sh") | nindent 12 }}
*/}}
{{- define "nos3.entrypointVolumeMount" -}}
- name: entrypoints
  mountPath: {{ .mountPath | default "/entrypoint.sh" }}
  subPath: {{ .entrypointKey }}
{{- end -}}

{{/*
Entrypoint volume definition
Usage: {{ include "nos3.entrypointVolume" (dict "root" .) | nindent 8 }}
*/}}
{{- define "nos3.entrypointVolume" -}}
- name: entrypoints
  configMap:
    name: {{ include "nos3.fullname" (dict "root" .root "component" "entrypoints") }}
    defaultMode: 0555
{{- end -}}

{{/*
NOS Engine config volume definition
Usage: {{ include "nos3.nosEngineConfigVolume" (dict "root" .) | nindent 8 }}
*/}}
{{- define "nos3.nosEngineConfigVolume" -}}
- name: nos-engine-config
  configMap:
    name: {{ include "nos3.fullname" (dict "root" .root "component" "nos-engine-config") }}
{{- end -}}

{{/*
Generate initContainers from a waitFor list
Usage: {{ include "nos3.initContainers" (dict "root" . "waitFor" $svc.waitFor) | nindent 8 }}
*/}}
{{- define "nos3.initContainers" -}}
{{- if .waitFor }}
initContainers:
{{- range $dep := .waitFor }}
  - name: wait-for-{{ $dep.service }}
    image: busybox:1.36
    {{- if eq (default "tcp" $dep.type) "http" }}
    command: ['sh', '-c', 'until wget -q --spider --timeout=2 http://{{ $dep.service }}:{{ $dep.port }}{{ $dep.path | default "/" }}; do echo "waiting for {{ $dep.service }}"; sleep 5; done']
    {{- else }}
    command: ['sh', '-c', 'until nc -z {{ $dep.service }} {{ $dep.port }}; do echo "waiting for {{ $dep.service }}:{{ $dep.port }}"; sleep 5; done']
    {{- end }}
{{- end }}
{{- end }}
{{- end -}}
