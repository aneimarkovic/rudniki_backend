# PointModel Dokumentacija
## Atributi:
- `lat`
- `lon`

## Metode:
- `getFromBsonDocument`
  Funkcija zgradi UserModel objekt iz pridobljenega BSON dokumenta
- `extractDoubleFromBSON`
 Funkcija, ki prejme ime polja in ga pretvori v double, če polja ni vrne error
- `convertToBsonDocument`
  Funkcija zgradi BSON dokument iz trenutnega UserModel objekta
    