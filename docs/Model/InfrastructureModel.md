# InfrastructureModel dokumentacija

## Atributi:

- `brand`:
  Ime stroja
- `model`:
  Model strojas
- `IDNumber`:
  ID številka stroja
- `avgFuelConsumption`:
  Povprečna poraba stroja
- `status`:
  Status stroja
- `lastMaintenance`:
  Zadnji servis/vzdrževanje
- `operatingHours`:
  Čas delovanja stroja
- `kilometer`:
  Št. prevoženih kilometrov

## Metode:

- `getFromBsonDocument`:
  Convert iz bson dokumenta v objekt
- `extractStringFromBSON`:
  pridobi string iz BSON
- `extractDoubleFromBSON`:
  pridobi double iz BSON
- `extractIntFromBSON`:
  pridobi int iz BSON
- `extractDateFromBSON`:
  pridobi timeStamp iz BSON
- `convertToBsonDocument`:
  pretvori objekt v BSON dokument
- `validateInfrastructure`:
  validira podatke v objektu
