# InfrastructureModel dokumentacija

## Atributi:
- `mineId`
    Hrani index rudnika, kateremu borderji pripadajo
- `points`
    Vektor točk, ki tvori polygon kar je meja rudnika
## Metode:
- `getFromBsonDocument`:
  Convert iz bson dokumenta v objekt
- `extractDoubleFromBSON`:
    pridobi double iz BSON
- `convertToBsonDocument`:
    pretvori objekt v BSON dokument