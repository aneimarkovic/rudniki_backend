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
- `type`:
    Vrsta rudnika
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
