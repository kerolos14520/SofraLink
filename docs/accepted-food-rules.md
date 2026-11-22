# Sample Accepted Food Rules — Academic Prototype

**Important:** This is an illustrative configuration for classroom testing only. It is not an official policy issued by any humanitarian organization and must not be used to approve real donations.

## Sample categories
The team may use these categories in simulated records:
- Rice
- Pasta
- Flour
- Dry beans and lentils
- Canned legumes
- Canned vegetables
- Cooking oil (sealed retail containers)
- Sugar and salt (sealed packages)

## Sample eligibility checks
A sample donation may be considered eligible only when:
1. `isSealed == true`.
2. `isNonPerishable == true`.
3. `totalQuantity > 0`.
4. `availableQuantity` is between 0 and `totalQuantity`.
5. `expiryDate` is a valid ISO date and is not in the past.
6. `category`, `unit`, `location`, and `donorName` are present.
7. `productBatchNumber` is recorded where available; if missing, the system may produce a warning for the academic prototype.

## Configuration and responsibility
- Member 3 documents the sample policy with Member 4.
- Member 4 implements field and date validation.
- A real receiving organization would need to supply its own authorized category list, minimum remaining shelf-life requirements, packaging rules, and inspection process.
- SofraLink only checks the data entered into the prototype. It does not certify food safety.
