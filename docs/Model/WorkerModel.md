# WorkerModel dokumentacija

## Atributi:

- `firstName`:
    Ime delavca
- `lastName`:
    Priimek delavca
- `birthDate`:
    Rojstni datum delavca
- `IDNumber`:
    ID številka delavca
- `Type`:
    Funkcija delavca (kaj dela)
- `Salary`:
    Plača delavca

## Metode:

- `getFromBsonDocument`:
  Convert iz bson dokumenta v objekt
- `extractStringFromBSON`:
  pridobi string iz BSON
- `extractDoubleFromBSON`:
  pridobi double iz BSON
- `extractIntFromBSON`:
  pridobi int iz BSON
- `convertToBsonDocument`:
  pretvori objekt v BSON dokument
- `validateWorkers`:
  validira podatke v objektu
