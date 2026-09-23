# Working on the Wings crypto lessons

- This is an interactive learning project. Implement only the phase the user has authorized, then wait for their result before advancing.
- Keep each phase in its own `lessons/NN_name/` directory with independent source, `prj.conf`, `CMakeLists.txt`, and README. Do not replace an earlier lesson when adding a later one.
- Preserve the original root application's source/configuration as a historical snapshot unless the user asks to change it.
- Earlier lesson fixes are allowed when needed, but explain the change and update the lesson's documentation. Preserve user experiments or document their restoration to a baseline.
- Explain concepts before introducing code. Each lesson README must cover purpose, concepts, files/configuration, code walkthrough, build/flash, expected output, one or two experiments, and the stopping point.
- Verify APIs and Kconfig against the installed SDK. The current baseline is NCS 2.4.0 / Zephyr v3.3.99-ncs1 / `nrf5340dk_nrf5340_cpuapp`.
- Use PSA/vendor cryptography; do not implement primitives or program CryptoCell registers manually. Public test keys must be labelled as non-production.
- Distinguish successful compilation, programming verification, and device runtime verification. Update `docs/PROGRESS.md` honestly and keep supervisor claims consistent with the evidence.
- Maintain `lesson.sh` and the VS Code application list when adding a runnable lesson. Do not add code for unapproved future phases.
- External-module support requires its exact board/module identity, power/voltage, and pinout. The DK's debugger model alone does not establish the correct firmware board configuration.
