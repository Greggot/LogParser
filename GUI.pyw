#**************************************************************************************************
# Includes
#**************************************************************************************************

from tkinter import *
from tkinter import ttk
from tkinter import messagebox
from tkinter.filedialog import askopenfilename
from tkinter.filedialog import asksaveasfilename
import subprocess
import os
import shutil

#**************************************************************************************************
# Definitions
#**************************************************************************************************

path = ''
prods = ['Delphi', 'TRW']  # Список производителей
servs = {'Delphi' : 'ReadMemoryByAddress', 'TRW' : 'TransferData'}  # Словарь, в котором обращение к элементам идёт не по целочисленному индексу, а по строкам

#**************************************************************************************************
# Window Structures (Widgets)
#**************************************************************************************************

os.system('mkdir temp')         # Создание временной папки, в которую по нотации сохраняются временные файлы

root = Tk()
root.title("Parser")                # Название окна
root.geometry('640x300+100+100')    # Размеры окна и расстояние от краёв экрана
root.resizable(False, False)        # Запрет на изменение размеров окна

SettingsFrame = ttk.LabelFrame(root, text = 'Settings', width = 300)            # Рамка, внутри которой находится настройки
SettingsFrame.pack(fill = X)

ChooseFrame = Frame(SettingsFrame, borderwidth = 0, highlightthickness = 0)     # Рамки выбора производителя и режима
ChooseFrame.pack(side = TOP, pady = 5)

UpperChoose = Frame(ChooseFrame, borderwidth = 0, highlightthickness = 0)       # Рамка выбора производителя
UpperChoose.pack(side = TOP)

DownChoose = Frame(ChooseFrame, borderwidth = 0, highlightthickness = 0)        # Рамка выбора режима. Все раки нужны, чтобы нормально отобразить элементы в центре
DownChoose.pack(side = TOP, pady = 5)

ttk.Label(UpperChoose, text = 'Producer:').pack(side = LEFT)        # Надпись "Производитель"
ttk.Label(DownChoose, text='Service:    ').pack(side = LEFT, pady = 5)  # Надпись "Режим"

ProdBox = ttk.Combobox(UpperChoose, values = prods, state = "readonly")             # Выбор производителя
ProdBox.pack(side = RIGHT, padx = 10)
ServBox = ttk.Combobox(DownChoose, values = None, state = "readonly")    # Выбор режима
ServBox.pack(side = RIGHT, pady = 5, padx = 10)
ServBox.set("-Select servise-")     # Изначальное значение выбора режима

#**************************************************************************************************
# Procedure ServsChange():     
#**************************************************************************************************

# Изменение списка сервисов в зависимости от производителя #

def ServsChange(event):     
    temp = servs[ProdBox.get()]
    ServBox.set("-Select servise-")
    ServBox['values'] = temp
    
ProdBox.bind("<<ComboboxSelected>>", ServsChange)   # Событие выбора варианта производителя вызывает ServsChange

SelectFrame = Frame(SettingsFrame, borderwidth = 0, highlightthickness = 0)     #Рамка выбора файла
SelectFrame.pack(side = BOTTOM, pady = 5)

path = StringVar()      # Тектовая переменная с тектом из последующего поля

PathEntry = Entry(SelectFrame, textvariable = path, width=100)         # Текстовое поле, хранящее путь к файлу
PathEntry.pack(side = BOTTOM, pady = 5, padx = 5)

#**************************************************************************************************
# Procedure setInputLog()
#**************************************************************************************************

# Считывание пути к входному файлу  #

def setInputLog():
    Path = askopenfilename(
        filetypes=((".log files", "*.log"),))   # Открывать только файлы формата .txt
    PathEntry.delete(0, END)                    # Стереть старый путь
    PathEntry.insert(0, Path)                   # Вставить новый

CheckFrame = Frame(SelectFrame, borderwidth = 0, highlightthickness = 0)
CheckFrame.pack(side = BOTTOM, pady = 5)

SelectFileButton = Button(SelectFrame, text = "Select a file", command = setInputLog)   # Кнопка, считывающая путь к файлу
SelectFileButton.pack(side = TOP, pady = 5, padx = 5)

showTable = IntVar()    # Переменная, хранящая выбор из CheckButton
Checkbutton(CheckFrame, text = "Show Table", variable = showTable).pack(side = LEFT, pady = 5, padx = 5)    # Галочка

#**************************************************************************************************
# Procedure Parse()
#**************************************************************************************************

# Запускает одно из окон на выбор #

def Parse():
    if(showTable.get() == 1 and path.get() != ''):  # Если галочка стоит и выбран файл, то создаётся окно
        proc = subprocess.Popen('LogParser ' + path.get(),  creationflags = subprocess.SW_HIDE, shell = True)   # Подготовить данные для окна вызовом программы
        proc.wait()                                                                                             # Обрабатывающей лог.log
        Table = Tk()
        Table.title("Table")                # Название окна
        Table.geometry('850x300+860+200')    # Размеры окна и расстояние от краёв экрана
        Table.resizable(False, False)        # Запрет на изменение размеров окна

        cmbBoxfile = open('temp/ID.txt', 'r')   # Открытие временного файла, созданного посредством LogParser
        cmbBoxValues = cmbBoxfile.read()

        IDChoosebox = Text(Table, width = 48, height = 1)  # Текст со списком ID для визуального анализа блоков, отвечающих двигателю и поиска диагностики
        IDChoosebox.insert(INSERT, 'Enter Multiple IDs Here')
        IDChoosebox.pack(side = TOP, padx = 7)

        ServerBox = ttk.Combobox(Table, values = cmbBoxValues, state = "readonly")             # Выбор ID сервера
        ServerBox.pack(side = TOP, padx = 10)   
        ServerBox.set("-Select Server-")
        ClientBox = ttk.Combobox(Table, values = cmbBoxValues, state = "readonly")             # Выбор ID клиента
        ClientBox.pack(side = TOP, padx = 10)
        ClientBox.set("-Select Client-")

#**************************************************************************************************
# Procedure ParseTable()
#**************************************************************************************************

# Запускает выбранный парсер для выбранного файла #

        def ParseTable():       # Определяется здесь, потому что зависит от комбобоксов в этом конкретном созданном окне. Аналогично для последующей функции
            outputPath = asksaveasfilename(filetypes=[("Binary File", "*.bin")])    # Диалоговое окно спрашивающее путь к файлу сохранения
            if(outputPath != ''):
                proc = subprocess.Popen('Parser' + ProdBox.get() + ServBox.get() + ' ' + os.getcwd() + '\\temp\dataLOGtemp.txt ' + outputPath + '.bin',  creationflags = subprocess.SW_HIDE, shell = True)  # Запуск программы извне
                proc.wait()     # Ожидание конца выполнения
                if os.path.isfile(outputPath + '.bin'):          # Если после выполнения нашёлся бинарный файл с нужным именем, то програма выполнилась правильно 
                    messagebox.showinfo('Success', 'File has been generated at ' + outputPath + '.bin')  # Сообщение с информацией об успехе
                else:
                    messagebox.showerror('Error', 'File hasn\'t been generated!')   # Сообщение-ошибка
        
        button = Button (Table, text = "   Parse   ", command = ParseTable)
        button.pack(side = BOTTOM, pady = 5)

#**************************************************************************************************
# Procedure SaveLog()
#**************************************************************************************************

# Сохранение таблицы-лога в формате *.txt #

        def SaveLog():
            thing = str(IDChoosebox.get(1.0, END)[0:-1])
            temp = 'Enter Multiple IDs Here'

            if(temp.find(thing) == -1):
                outputPath = asksaveasfilename(filetypes=[("Text File", "*.txt")])  # Спросить путь, куда сохранить
                if(outputPath != ''):
                    idSort = subprocess.Popen('IDLogParser ' + path.get() + ' ' + thing ,  creationflags = subprocess.SW_HIDE, shell = True)
                    idSort.wait()       # Запустить выборку строк лога по ID, взяв их и комбобоксов
                    shutil.copyfile('temp/LOGtempsave.txt', outputPath + '.txt')
            else:
                outputPath = asksaveasfilename(filetypes=[("Text File", "*.txt")])  # Спросить путь, куда сохранить
                if(outputPath != ''):
                    if (ServerBox.get() != '-Select Server-' and ClientBox.get() != '-Select Client-'): # Если выбраны оба ID 
                        shutil.copyfile('temp/LOGtempsave.txt', outputPath + '.txt')
                    else:   # Иначе сохранить исходную цеую таблицу
                        shutil.copyfile('temp/LOGsave.txt', outputPath + '.txt')
        
        saveButton = Button (Table, text = "   Save file   ", command = SaveLog) # Кнопка вызывающая SaveLog
        saveButton.pack(side = BOTTOM, pady = 5)

        log = open('temp/LOG.txt', 'r') # Файл с логом-таблицей изначальный
        logdata = log.read()
        
        textbox = Text(Table, width = 92)   # Виджет текста, создаётся отключенным после помещении данных из файла
        textbox.insert(INSERT, logdata)     # Ширина измеряется в символах
        textbox.config(state=DISABLED)
        textbox.pack(side = LEFT)

        scrollbar = Scrollbar(Table)    # Скроллбар, пролистывающий вверх-вниз
        scrollbar.pack(side=LEFT, fill=Y)

        IDtextbox = Text(Table, width = 8)  # Текст со списком ID для визуального анализа блоков, отвечающих двигателю и поиска диагностики
        IDtextbox.insert(INSERT, cmbBoxValues)
        IDtextbox.pack(side = LEFT, padx = 7)
        
        textbox.config(yscrollcommand=scrollbar.set)
        scrollbar.config(command=textbox.yview)

#**************************************************************************************************
# Procedure serverIDselected()
#**************************************************************************************************

# Обновление таблицы в зависимости от выбора комбобокса #

        def serverIDselected(event):
            if(ClientBox.get() != '-Select Client-' and ClientBox.get() != ServerBox.get()):    # Если второй комбобокс был выбран и не равен текущему

                idSort = subprocess.Popen('IDLogParser ' + path.get() + ' ' + ClientBox.get() + ' ' + ServerBox.get() ,  creationflags = subprocess.SW_HIDE, shell = True)
                idSort.wait()       # Запустить выборку строк лога по ID, взяв их и комбобоксов
                
                textbox.config(state=NORMAL)    # Включить редактирование текста
                textbox.delete(1.0, END)        # Удалить предыдующее значение
                textbox.insert(INSERT, open('temp/LOGtemp.txt', 'r').read())   # Вставить значение из файла
                textbox.config(state = DISABLED)    # ОТключить редактирование текста

#**************************************************************************************************
# Procedure clientIDselected()
#**************************************************************************************************

# Обновление таблицы, см Procedure serverIDselected() #

        def clientIDselected(event):
            if(ServerBox.get() != '-Select Server-' and ClientBox.get() != ServerBox.get()):

                idSort = subprocess.Popen('IDLogParser ' + path.get() + ' ' + ClientBox.get() + ' ' + ServerBox.get() ,  creationflags = subprocess.SW_HIDE, shell = True)
                idSort.wait()
                
                textbox.config(state=NORMAL)
                textbox.delete(1.0, END)
                textbox.insert(INSERT, open('temp/LOGtemp.txt', 'r').read())
                textbox.config(state = DISABLED)
                
        ServerBox.bind("<<ComboboxSelected>>", serverIDselected)    # Событие выбора ID сервера
        ClientBox.bind("<<ComboboxSelected>>", clientIDselected)    # Событие выбора ID клиента
        
        Table.mainloop()
    else:   # Создание минималистичной версии окна без визуального отображения таблицы. Функции ParseTable() и SaveLog() аналогичны таковым в ветке if, но зависят от комбобоксов этого окна
        if(path.get() != ''):   # Если галочка не стоит и выбран файл
            proc = subprocess.Popen('LogParser ' + path.get(),  creationflags = subprocess.SW_HIDE, shell = True)
            proc.wait()     # Подготовить изначальную таблицу и список ID
            IDselect = Tk()
            IDselect.title("ID select")                # Название окна
            IDselect.geometry('300x170+800+200')    # Размеры окна и расстояние от краёв экрана
            IDselect.resizable(False, False)        # Запрет на изменение размеров окна

            cmbBoxfile = open('temp/ID.txt', 'r')   # Аналогичное чтение информации для блоков выбора ID сервера и клиента
            cmbBoxValues = cmbBoxfile.read()
        
            IDChoosebox = Text(IDselect, width = 48, height = 1)  # Текст со списком ID для визуального анализа блоков, отвечающих двигателю и поиска диагностики
            IDChoosebox.insert(INSERT, 'Enter Multiple IDs Here')
            IDChoosebox.pack(side = TOP, padx = 7)
            
            ServerBox = ttk.Combobox(IDselect, values = cmbBoxValues, state = "readonly")             
            ServerBox.pack(side = TOP, pady = 5)
            ServerBox.set("-Select Server-")
            ClientBox = ttk.Combobox(IDselect, values = cmbBoxValues, state = "readonly")             
            ClientBox.pack(side = TOP, pady = 5)
            ClientBox.set("-Select Client-")

#**************************************************************************************************
# Procedure ParseTable()
#**************************************************************************************************

# Запуск выбранного парсера #

            def ParseTable():
                thing = str(IDChoosebox.get(1.0, END)[0:-1])
                temp = 'Enter Multiple IDs Here'

                if(temp.find(thing) == -1):
                    outputPath = asksaveasfilename(filetypes=[("Binary File", "*.bin")])
                    if(outputPath != ''):
                        idSort = subprocess.Popen('IDLogParser ' + path.get() + ' ' + IDChoosebox.get(1.0, END) ,  creationflags = subprocess.SW_HIDE, shell = True)
                        idSort.wait()
                        proc = subprocess.Popen('Parser' + ProdBox.get() + ServBox.get() + ' ' + os.getcwd() + '\\temp\dataLOGtemp.txt ' + outputPath + '.bin',  creationflags = subprocess.SW_HIDE, shell = True)  # Запуск программы извне
                        print('Parser' + ProdBox.get() + ServBox.get() + ' ' + os.getcwd() + '\\temp\dataLOGtemp.txt ' + outputPath + '.bin')
                        proc.wait()     # Ожидание конца выполнения
                        if os.path.isfile(outputPath + '.bin'):          # Если после выполнения нашёлся бинарный файл с нужны именем, то програма выполнилась правильно 
                            messagebox.showinfo('Success', 'File has been generated at ' + outputPath + '.bin')  # Сообщение с информацией об успехе
                        else:
                            messagebox.showerror('Error', 'File hasn\'t been generated!')   # Сообщение-ошибка
                elif(ServerBox.get() != '-Select Server-' and ClientBox.get() != '-Select Client-'):  
                    outputPath = asksaveasfilename(filetypes=[("Binary File", "*.bin")])
                    if(outputPath != ''):
                        idSort = subprocess.Popen('IDLogParser ' + path.get() + ' ' + ClientBox.get() + ' ' + ServerBox.get() ,  creationflags = subprocess.SW_HIDE, shell = True)
                        idSort.wait()
                        proc = subprocess.Popen('Parser' + ProdBox.get() + ServBox.get() + ' ' + os.getcwd() + '\\temp\dataLOGtemp.txt ' + outputPath + '.bin',  creationflags = subprocess.SW_HIDE, shell = True)  # Запуск программы извне
                        print('Parser' + ProdBox.get() + ServBox.get() + ' ' + os.getcwd() + '\\temp\dataLOGtemp.txt ' + outputPath + '.bin')
                        proc.wait()     # Ожидание конца выполнения
                        if os.path.isfile(outputPath + '.bin'):          # Если после выполнения нашёлся бинарный файл с нужны именем, то програма выполнилась правильно 
                            messagebox.showinfo('Success', 'File has been generated at ' + outputPath + '.bin')  # Сообщение с информацией об успехе
                        else:
                            messagebox.showerror('Error', 'File hasn\'t been generated!')   # Сообщение-ошибка
            
            button = Button (IDselect, text = "   Parse   ", command = ParseTable)
            button.pack(side = BOTTOM, pady = 5)

#**************************************************************************************************
# Procedure SaveLog()
#**************************************************************************************************

# Сохранение лога-таблицы в виде файла #

            def SaveLog():
                thing = str(IDChoosebox.get(1.0, END)[0:-1])
                temp = 'Enter Multiple IDs Here'

                if(temp.find(thing) == -1):
                    outputPath = asksaveasfilename(filetypes=[("Text File", "*.txt")])
                    if(outputPath != ''):
                            idSort = subprocess.Popen('IDLogParser ' + path.get() + ' ' + thing ,  creationflags = subprocess.SW_HIDE, shell = True)
                            idSort.wait()
                            shutil.copyfile('temp/LOGtempsave.txt', outputPath + '.txt')
                else:
                    outputPath = asksaveasfilename(filetypes=[("Text File", "*.txt")])
                    if(outputPath != ''):
                        if (ServerBox.get() != '-Select Server-' and ClientBox.get() != '-Select Client-'):
                            idSort = subprocess.Popen('IDLogParser ' + path.get() + ' ' + ClientBox.get() + ' ' + ServerBox.get() ,  creationflags = subprocess.SW_HIDE, shell = True)
                            idSort.wait()
                            shutil.copyfile('temp/LOGtempsave.txt', outputPath + '.txt')
                        else:
                            shutil.copyfile('temp/LOGsave.txt', outputPath + '.txt')
                
            saveButton = Button (IDselect, text = "   Save file   ", command = SaveLog)
            saveButton.pack(side = BOTTOM, pady = 5)
    
message_button = Button(root, text = "Filter", command = Parse)    # Кнопка, запускающая Parse()
message_button.pack()

root.mainloop()         # Функция, обновляющая виджеты на экране

shutil.rmtree('temp')
