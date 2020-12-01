# Запуск

`make objects`

`make ParserDelphiReadMemoryByAddress`

`make ParserTRWTransferData`

`make LogParser`

`make IDLogParser`

`make Parse`

# Описание консольных приложений

Пармеры имеют формат `Parser-mark-command_name`.

### `ParserDelphiReadMemoryByAddress`

Парсит логи DAF по команде 0х23 после последнего hard reset (0x11). Принимает на вход путь к текстовому файлу формата 8 байт данных в строке, путь к выходному бинарному файлу. 

Пример: `ParserDelphiReadMemoryByAddress LogDAF.txt I:\logs\DAF\DAFlog_1.20.2020.bin`.

### `ParserTRWTransferData`

Аналогично парсит логи Volvo. 

Пример: `ParserTRWTransferData C:\Users\Name\Desktop\LogVolvo.txt D:\Volvo\Vlog_1.bin`.

### `LogParser` и `IDLogParser`

Программы вынимают из лога формата .log полезные данные (ID-DATA-TIME & DATA) и выводят в файлы temp/LOG.txt, temp/dataLOG.txt. Если используется `IDLogParser`, то к именам файлов добавляется temp. В конце указывается формат лога: `0` для разделителя-пробела, всё остальное - для таба; номер столбца (с нуля) в котором хранится время, ID, длина данных в строке или `255`, если предполагается, что столбца нет и длина всегда равна 8; в последнюю очередь указывается номер первого столбца данных.

Примеры:

`LogParser Unknown.log 0 1 2 3 4` - разделители - пробелы, сам лог выглядит примерно так: "RX 23.02.12.567 0FC00300 1 FF"

`IDLogParser Unknown.log 18FCF700 18FC00F7 1 2 5 255 6` - Разделители - табы, лог: "RX 00001 23.02.12.567  29  0 18FF3400 FF FF FF FF 24 A4 FF FF"
