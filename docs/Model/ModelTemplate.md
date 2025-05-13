# ModelTemplate Dokumentacija
## Na hitro
To je Class, ki je template za vse modele mongodb v naši kodi.

## Atributi:
- `id`: 
    Vsi vnosi v mongodb imajo id zato ga shranimo tu.
- `created`: 
    Shrani čas, ko se je ustvaril objekt
- `modified`: 
    Shrani čaš najnovejše spremembe 

## Metode:

- `extractStringFromBSON`
    Funkcija vzame ime polja v BSON in ga da v string format za uporabo.
Če polja ni vrne error
- `extractDateFromBSON`
    Funkcija vzame ime polja v BSON in ga da v date format za uporabo
    Če polja ni vrne error
- `touch` 
    Način, da naredimo posodobitev private spremenljivke modified 

### Uporabljena knjižnica Boost.beast
[Boost dokumentacija](https://live.boost.org/)
