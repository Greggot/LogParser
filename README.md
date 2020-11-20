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

Программы вынимают из лога формата .log, в котором символами-разделителями являются пробелы, поленые данные (ID-DATA-TIME & DATA) и выводят в файлы temp/LOG.txt, temp/dataLOG.txt. Если используется `IDLogParser`, то к именам файлов добавляются ID сервера и ID клиента. 

Примеры:

`LogParser Unknown.log`

`IDLogParser Unknown.log 18FCF700 18FC00F7`
