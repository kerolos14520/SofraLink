# Project Charter — SofraLink

**Project:** Smart Humanitarian Food Donation Management & Traceability System  
**Type:** Academic software development project  
**Team size:** 6 members  
**Primary language:** C++17  
**Status:** Academic prototype in setup phase

## Purpose and vision
SofraLink aims to provide an organized, transparent prototype for managing food donations from multiple sources, checking their submitted information against sample acceptance rules, maintaining inventory, generating proposed matches, and recording documented handover to an authorized humanitarian organization.

## Objectives
1. Centralize donation and receiving-organization request records.
2. Validate required fields, quantities, packaging flags, and dates.
3. Monitor product quantity, unit, batch number, and expiry date.
4. Prioritize eligible donations by expiry date and request urgency.
5. Generate proposed matches without exceeding available or remaining quantities.
6. Preserve donation, request, and match status information.
7. Save and reload records locally.
8. Generate summary reports.
9. Demonstrate C++ data structures and algorithms through a working prototype.

## Team roles
| Member | Responsibility |
|---|---|
| 1 — Kerolos | Shared models, MatchingEngine, C++ API and integration |
| 2 | DonationManager |
| 3 | RequestManager and accepted-food rules document |
| 4 | Validation and SearchService |
| 5 | Frontend |
| 6 | JsonStorage and ReportService |

## Constraints and assumptions
- Six team members with different programming skill levels.
- C++17 and Data Structures & Algorithms are core academic requirements.
- Local JSON persistence for the initial prototype.
- The frontend uses a lightweight C++ HTTP API.
- All sample data is simulated.
- No official humanitarian partnership or real logistics operation is assumed.

## Risks and responses
| Risk | Response |
|---|---|
| Inconsistent shared interfaces | Freeze models, enums and function signatures before parallel development |
| AI-generated code does not compile | Require builds, tests, review and member explanation |
| Git merge conflicts | Separate feature branches and Pull Requests |
| Invalid submitted data | Central Validation service |
| Scope expansion | Complete the core workflow before optional enhancements |
| Lack of partner data | Use labeled sample rules and simulated data |

## Success criteria
- CMake build succeeds from a clean clone.
- Valid donations and requests can be created and queried.
- Invalid values are rejected with clear errors.
- Matching prevents over-allocation and excludes expired donations.
- Data can be saved and reloaded.
- Frontend communicates with C++ API.
- Critical tests pass and all six members can explain their work.

## Approval
Project lead: Kerolos Mansour  
Academic supervisor: To be completed  
Approval date: To be completed
