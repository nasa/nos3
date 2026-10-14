# NASA Operational Simulator for Space Systems (NOS3)

NOS3 is a suite of tools developed by NASA's Katherine Johnson Independent Verification and Validation (IV&V) Facility to aid in areas such as software development, integration & test (I&T), mission operations/training, verification and validation (V&V), and software systems check-out. 
NOS3 provides a software development environment, a multi-target build system, an operator interface/ground station, dynamics and environment simulations, and software-based models of spacecraft hardware.

# Documentation

[NOS3 - ReadTheDocs](https://nos3.readthedocs.io/en/latest/)

## Standalone CI startup corrections

This branch addresses [#888](https://github.com/nasa/nos3/issues/888),
[#889](https://github.com/nasa/nos3/issues/889),
[#890](https://github.com/nasa/nos3/issues/890), and
[#891](https://github.com/nasa/nos3/issues/891) against NOS3 1.7.5
(`5a3bdee6be9a2c67fdf994ae6db56d5c60395302`). It does not add distributed
deployment tooling, SpaceCop, or the separate OSAL/PSP/CryptoLib source backports.

- Preparation loads `scripts/env.sh` once and reports missing environment files
  and required preparation failures. Optional Igniter installation remains optional.
- CI startup establishes the spacecraft network and NOS Engine before their
  clients, then starts 42 clients in the stock IPC accept order. Radio receives
  its `radio-sim` alias; its DNS and TCP/8010 listener must be ready before
  CryptoLib starts. The flight container provides `nos-fsw` DNS before radio
  resolves its destination, but a bounded gate defers the flight executable
  until dependencies are ready.
- CI readiness uses bounded Docker operations and passive Linux socket tables,
  not test connections to single-client listeners. Started containers are
  monitored for exit, restart, identity changes, and declared unhealthy state
  during startup. Headless COSMOS runs its command server in the foreground. A
  failed check exits nonzero without performing mission-wide cleanup; inspect
  remaining containers and logs for diagnosis.
- CI system tests require fresh, advancing 42 truth telemetry and successful
  SAMPLE NOOP commands over both DEBUG and RADIO/CryptoLib, with unchanged error
  counters. Cached telemetry or a debug-only success cannot pass the radio test.

The standard **graphical `make launch` workflow and Vagrant GUI defaults are
unchanged**. `scripts/ci_launch.sh` defaults to headless COSMOS and disables 42
rendering only in a fresh `~/.nos3/42/NOS3InOut-ci.*` runtime copy. Original
generated inputs remain unchanged. `--use-cosmos-gui` uses the host `DISPLAY`
and preserves the configured 42 graphics choice. Headless `--use-yamcs` does not
open a browser automatically.

After the normal preparation, configuration, and build steps:

```bash
# Headless startup plus command/telemetry tests (COSMOS configuration required):
make ci-launch

# Startup only, with explicit graphical COSMOS and 42's configured graphics:
./scripts/ci_launch.sh --use-cosmos-gui
./scripts/system_tests.sh

# Focused offline regression tests (Python 3, Bash, and Ruby required):
python3 -B -m unittest discover -s tests/launch -v
```

`NOS3_CI_TIMEOUT_SECONDS` controls each startup readiness deadline (default 180,
range 1–900 seconds). Required container images must already be cached; the
headless COSMOS container installs Xvfb only if missing. The command/telemetry
test has its own bounded deadline and requires the stock `SAMPLE`,
`SAMPLE_RADIO`, and `SIM_42_TRUTH` configuration and an otherwise idle command
path. YAMCS startup checks do not constitute a COSMOS system-test pass.

These are startup checks, not a persistent runtime supervisor or a repair for
NOS Engine shutdown/reconnection defects. Existing `make stop` cleanup is broad;
use a dedicated NOS3 environment. CI runtime directories are retained for
diagnosis, including on failure; remove only the specific run directory after
its containers have stopped and its output is no longer needed.

The automated regression suite uses isolated command/API fixtures. An additional
opt-in real-Docker test verifies passive socket/DNS checks and lifecycle rejection:

```bash
NOS3_TEST_DOCKER_IMAGE=ivvitc/nos3-64:20260619 \
    python3 -B tests/launch/test_docker_readiness.py -v
```

It creates and removes only uniquely labeled test containers and an internal
network, using a cached image without publishing ports or mounting host paths.
Neither fixture suite is a full live NOS3 mission qualification. Run the bounded
COSMOS command/telemetry test against the actual configured mission before
claiming end-to-end operation.

## Issues and Feature Requests

Please report issues and request features on the GitHub tracking system - [NOS3 Issues](https://www.github.com/nasa/nos3/issues).

If you would like to contribute to the repository, please complete this [NASA Form][def] and submit it to gsfc-softwarerequest@mail.nasa.gov with Justin.R.Morris@nasa.gov CC'ed.
Next, please create an issue describing the work to be performed noting that you intend to work it, create a related branch, and submit a pull request when ready. When complete, we will review and work to get it integrated.

If this project interests you or if you have any questions, please feel free to open an issue or discussion on the GitHub, contact any developer directly, or email `support@nos3.org`.

[def]: https://github.com/nasa/nos3/files/14578604/NOS3_Invd_CLA.pdf "NOS3 NASA Contributor Form PDF"

## License

This project is licensed under the NOSA 1.3 (NASA Open Source Agreement) License. 

## Versioning

We use [SemVer](http://semver.org/) for versioning. For the versions available, see the tags on this repository.
