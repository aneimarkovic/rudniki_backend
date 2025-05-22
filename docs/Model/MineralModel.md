# MineralModel dokumentacija

## Atributi:

- `name`:
  Ime minerala
- `min`:
  Minimalna meja ocenjenih zalog minerala
- `max`:
  Maximalna meja ocenjenih zalog minerala
- `grade`:
  Ocena "čistosti" minerala

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
- `validateMinerals`:
  validira podatke v objektu
