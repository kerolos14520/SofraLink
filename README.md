# SofraLink

**Smart Humanitarian Food Donation Management & Traceability System**

SofraLink is an academic C++17 prototype for organizing humanitarian food donations, validating donation information against sample acceptance rules, managing donation and request records, prioritizing eligible donations, and maintaining traceability up to documented handover to an authorized humanitarian organization.

> **Prototype boundary:** This repository uses simulated data only. It does not transport aid across borders, distribute food inside Palestine, certify food safety, or claim an official partnership with any humanitarian organization.

## Main features planned

- Register and manage packaged, sealed, non-perishable food donations.
- Manage receiving-organization food requests.
- Validate required fields, quantities, packaging flags, and dates.
- Search and filter donations and requests.
- Prioritize eligible donations by expiry date and request urgency.
- Generate proposed donation/request matches without over-allocating quantities.
- Track donation and match statuses.
- Persist records using local JSON files.
- Generate summary reports.
- Provide a lightweight web interface backed by a C++ HTTP API.

## Technology

- C++17
- CMake
- HTML, CSS, vanilla JavaScript
- nlohmann/json (persistence module)
- cpp-httplib (HTTP API)
- Git and GitHub

The HTTP and JSON dependencies must be added centrally by Member 1 in CMake. Do not add competing libraries independently.

## Build the starter scaffold

Requirements: CMake 3.16+ and a C++17 compiler.

```bash
cmake -S . -B build
cmake --build build
```

Run:

- Linux/macOS: `./build/SofraLink`
- Windows: `build\Debug\SofraLink.exe` or the matching configuration folder.

The initial target is intentionally small and dependency-free. Members implement their assigned modules in feature branches.

## Important documents

- `AGENTS.md` — instructions for AI coding tools.
- `CONTRIBUTING.md` — Git workflow and collaboration rules.
- `docs/project-scope.md` — agreed project scope and boundaries.
- `docs/architecture.md` — high-level architecture.
- `docs/shared-interface-contract.md` — fixed model names, fields, enums, and function signatures.
- `docs/api-contract.md` — routes used by the frontend.
- `docs/accepted-food-rules.md` — sample, non-official acceptance rules.
- `docs/team-tasks/` — six individual task PDFs sent to the team.

## Branch workflow

- `main`: stable milestones only.
- `dev`: shared integration branch.
- Each member uses their assigned feature branch and opens a Pull Request into `dev`.
- Do not commit directly to `main` or `dev`.

See `CONTRIBUTING.md` and each member's task PDF.
