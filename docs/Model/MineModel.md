# MineModel dokumentacija

## Atributi:

- `id`:
    Id rudnika
- `name`:
    Ime rudnika
- `ownerId`:
  ID lastnika rudnika (uporabnik)
- `status`:
  Status rudnika
- `municipality`
    Občina rudnika
- `type`:
    Vrsta rudnika
- `startYear`
    Leto nastanka rudnika
- `endYear`
    Leto zaprtje rudnika
- `minerals`:
  Seznam mineralov v rudniku
- `infrastructure`:
  Seznam infrastrukture v rudniku
- `workers`:
  Seznam delavcev v rudniku

## Metode:

- `getFromBsonDocument`:
  Convert iz bson dokumenta v objekt
- `convertToBsonDocument`:
  pretvori objekt v BSON dokument
- `validateMineData`:
  validira podatke v objektu
- `mineStatusToString`
    Pretvori enum v string
- `mineTypeToString`
  Pretvori enum v string