# HTTP SERVER DOKUMENTACIJA
## Atributi:
- `ioContext`: 
    Orodje/mehanizem za delo z I/O operacijami.
- `acceptor`: 
    sprejme povezavo uporabnika, in uporabi `ioContext` za njeno obdelavo. 
- `socket`: 
    vtičnica, ki se ustvari in nastavi ob povezavi.

## Metode:
- `Konstruktor` 
    ustvari objekt. Vhodna parametra sta naslov in vrat, ki povesta, kje naj strežnik posluša
- `runServer`
    metoda, ki zažene strežnik, čaka na zahteve ob prihajajoči zahtevi ustvari novo nit in pokliče metodo `getRequest`, ki upravlja njeno nadaljno obdelavo.
- `parseRequest`
    začasna oz. testna metoda, ki je bila ustvarjena z namenom testiranja kode ali strežnik pravilno sprejme zahtevo in vrne odgovor.
- `getRequest` 
    metoda, ki pošilja zahtevo routerju, ki gre skoz seznam in se odloči kdo in kako bo metodo obdelal.
- `send` 
    metoda, ki kot parametra prejme vtičnik in objekt odgovora, ki ga pošlje nazaj uporabniku.

### Uporabljena knjižnica Boost.beast
[Boost dokumentacija](https://live.boost.org/)
