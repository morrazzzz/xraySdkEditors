Данный репозиторий представляет собой модифицированную версию редакторов из сдк движка X-Ray. За основу взят код sdk 0.8 от RedPanda (BearIvan): 
https://github.com/RedPandaProjects/XRayEngine

### Основные изменения:

* Исправление ошибок sdk 0.8, попытка привести сдк к работоспособному состоянию
* Реализована поддержка sdk 0.4 и форматов тч для LE (совместимость с sdk 0.7 и зп также оставлена)
* Поддержка x64
* Не требует установки visual c++ для запуска
* Вырезано все кроме редакторов
* Собирается в visual studio 2019 без всяких дополнительных компонентов

Из сдк 0.8 не были перенесены Gizmo и фича "Играть в редакторе". Для отладки рекомендуется использовть конфигурацию Development, для полноценного использования - конфигурацию Release.

### Установка:
Распаковать в папку с сдк. Запускать в папке с бинами нужный exe.