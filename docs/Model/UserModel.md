# ModelTemplate Dokumentacija
## Na hitro
Implemetacija User modela za mongodb bazo.

## Atributi:
- `username` 
- `password` 
- `email` 
- `birthDate`

## Metode:

- `getFromBsonDocument`
     Funkcija zgradi UserModel objekt iz pridobljenega BSON dokumenta
- `convertToBsonDocument`
    Funkcija zgradi BSON dokument iz trenutnega UserModel objekta
    
### Uporabljena knjižnica Boost.beast
[Boost dokumentacija](https://live.boost.org/)
