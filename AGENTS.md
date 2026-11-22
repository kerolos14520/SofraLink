# AGENTS.md — SofraLink AI Coding Instructions

Read this file, `docs/shared-interface-contract.md`, and your assigned task PDF in `docs/team-tasks/` before editing.

## Project context
SofraLink is a C++17 academic prototype for humanitarian food donation management and traceability up to documented handover to an authorized organization. The prototype uses simulated records and focuses on packaged, sealed, non-perishable products under sample acceptance rules.

It does not transport aid across borders, distribute food inside Palestine, certify food safety, or represent an official humanitarian organization.

## Rules
1. Use C++17 and namespace `sofralink` for C++ project code.
2. Use shared models, field names, enum values, and function signatures exactly as documented in `docs/shared-interface-contract.md` and the headers.
3. Never create duplicate `Donation`, `FoodRequest`, `MatchResult`, `ValidationResult`, or enum definitions.
4. Never rename a shared field, method, enum value, JSON key, or API route without approval from Member 1 and an updated contract.
5. Implement only the module assigned in your task PDF. Do not rewrite the whole repository or another member's module.
6. Keep business logic in C++. The frontend must call the C++ API; it must never read or write `data/runtime/*.json` directly.
7. JSON persistence belongs to `JsonStorage` (Member 6). Managers do not serialize JSON themselves.
8. Validate all incoming values. Frontend validation is for usability only; C++ validation is authoritative.
9. Do not add frameworks, databases, cloud services, or unrelated features without approval.
10. Do not leave placeholder functions that return success without implementing behavior.
11. Compile and test changed code. Report exact commands and actual results.
12. Use simulated data only. Never commit real donor, beneficiary, contact, credential, or personal location data.

## Shared conventions
- Dates: `YYYY-MM-DD`.
- Timestamps: ISO 8601 text.
- Quantities are positive integers paired with a `unit`; never add different units together.
- Request urgency: 1 (lowest) to 5 (highest).
- JSON keys: lowerCamelCase.
- Enum values in JSON: UPPER_SNAKE_CASE strings, never numeric ordinals.
- A proposed match is not approval, handover, or proof of delivery.

## Before coding
1. Inspect the existing repository and branch.
2. Read `docs/shared-interface-contract.md` and your assigned task PDF.
3. List the files you plan to create or modify.
4. Confirm that your planned changes do not overlap another member's ownership.

## Before finishing
1. Run the documented CMake build.
2. Run relevant unit tests, including invalid-input cases.
3. Review `git diff` and exclude unrelated files.
4. Summarize files changed, implementation, build/test commands, results, and limitations.
5. Push only your assigned feature branch and open a Pull Request into `dev`.

## AI behavior
You are an implementation assistant, not the project architect. Do not invent requirements or change the shared contract to make implementation easier. If a needed interface is missing or inconsistent, stop and report it to Member 1.
