# SofraLink — Project Scope

## Definition
SofraLink is an academic prototype for organizing humanitarian food donations, validating donation information against sample acceptance rules, managing donation and request records, prioritizing eligible donations, generating proposed matches, and tracking donation status up to documented handover to an authorized humanitarian organization.

## Included in the first version
1. Donation registration and lifecycle management.
2. Receiving-organization request management.
3. Validation of required fields, quantities, packaging flags, and dates.
4. Search, filtering, and expiry sorting.
5. Expiry-aware matching of eligible donations with compatible requests.
6. Prevention of over-allocation.
7. JSON persistence and summary reports.
8. A lightweight frontend connected to a C++ HTTP API.
9. Unit and integration tests.

## Excluded
- Actual cross-border transportation or distribution inside Palestine.
- Direct integration with border authorities or humanitarian organizations.
- Financial donations or payment processing.
- Live warehouse operations.
- Official partnership claims.
- Food-safety certification. The system checks entered information against sample rules only.
- Production authentication or mobile applications.

## Food assumptions
The academic prototype focuses on packaged, sealed, non-perishable products. The acceptance-rule document is illustrative and is not an official policy from any specific organization. Any real deployment would need rules and checks approved by qualified partners.

## Data
All repository data is simulated. Never use real donor, beneficiary, organization-contact, or personal-location information.
