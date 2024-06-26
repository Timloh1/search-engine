# Search engine
Программа производит поиск запросов в текстовых документов, рассчитывая релевантность для каждого запроса в каждом документе.
Программа получает данные о текстовых документах и запросах из json файлов config.json и requests.json, которые находятся в корневой папке проекта,
а ответы выводит в файл answer.json, который также помещает в корневую папку проекта.
При отсутствии хотя бы одного из файлов config.json и requests.json программа не будет работать, а файл answer.json программа создает автоматически.

## Описание проекта и пользовательская документация

### В проекте есть 3 основных класса:
1) ConverterJSON - служит для работы с json файлами;
2) InvertedIndex - служит для реализации инвертированной индексации документов;
3) SearchServer - служит для расчета релевантности запроса в каждом документе.

### Описание файлов config.json, requests.json и answer.json:
**config.json (пример описания)**

```
"config": {
    "name": "SearchEngine",
    "version": "0.1",
    "max_responses": 5
  },
  "files": [
    "../resources/file001.txt",
    "../resources/file002.txt",
    "../resources/file003.txt",
    "../resources/file004.txt",
    …
  ]
}
```
Программа выдаст ошибку и остановит свое выполнение при отсутствии ключа "config"

**requests.json (пример описания)**

```
{
  "requests": [
    "some words..",
    "some words..",
    "some words..",
    "some words..",
    …
  ]
}
```

**answer.json (пример описания)**

```
{
  "answers": {
    "request001": {
      "result": "true",
      "relevance": {
        "docid": 0, “rank” : 0.989,
        "docid": 1, “rank” : 0.897,
        "docid": 2, “rank” : 0.750,
        "docid": 3, “rank” : 0.670,
        "docid": 4, “rank” : 0.561
      }
    },
    "request002": {
      "result": "true",
      "docid": 0, “rank” : 0.769
    },
    "request003": {
      "result": "false"
    }
  }
}
```

### Описание методов классов:
**ConverterJSON**
1) ConverterJSON() - конструктор класса.
2) vector<string> GetTextDocuments() - возвращает вектор с содержанием каждого из текствых документов, пути
к которым описаны в config.json, при неправильном пути или отсутствии документа выводит ошибку, но продолжает свое выполнение.
3) int GetResponsesLimit() - возвращает максимальное количество возможных текстовых документов. Выдает ошибку при привышении лимита
и прекращает свое выполнение.
4) vector<string> GetRequests() - возвращает вектор запросов описанных в файле requests.json.
5) void putAnswers(vector<vector<RelativeIndex>> answers) - заполняет файл answer.json. Принимает на вход двумерный вектор элементов
типа RelativeIndexж
6) string getName() - возвращает имя проекта

**InvertedIndex**
1) InvertedIndex() = default - конструктор класса
2) int getCountDocs() - возвращает количестов документов.
3) vector<Entry> GetWordCount(const string& word) - метод определяет количество вхождений слова word в загруженной базе документов.
Получает word слово, частоту вхождений которого необходимо определить, возвращает подготовленный вектор типа Entry с частотой вхлждение слова в каждый из файлов.
4) map<string, vector<Entry>> getFreqDictionary() - возвращает частотный словарь вхождения слов.
5) void UpdateDocumentBase(vector<string> input_docs) - обновить или заполнить базу документов, по которой будем совершать
поиск. Получает содержимое документов.

**SearchServer**
1) SearchServer(InvertedIndex _index) - конструктор класса. Передаётся ссылка на класс InvertedIndex,
чтобы SearchServer мог узнать частоту слов встречаемых в запросею.
2) vector<vector<RelativeIndex>> search(const vector<string>& queries_input) - метод обработки поисковых запросов. Получает поисковые запросы взятые из файла
requests.json. Возвращает отсортированный список релевантных ответов для заданных запросов.

### Также для удобства создания  файлов config.json и requests.json были добавлены функции:
1) createStandartConfig();
2) createStandartRequests();
## Установка (Linux, Windows, Mac OS)

Клонирование репозитория 

```git clone https://github.com/Timloh1/search-engine.git```

## Поддержка
Если у вас возникли сложности или вопросы по использованию проекта, напишите на электронную почту ilya4nikitin@yandex.ru.

## Зависимости
Cтандарт языка c++ 20+    
Компилятор желательно msvc 2019

## Описание коммитов
| Название | Описание                                                        |
|----------|-----------------------------------------------------------------|
| build	   | Сборка проекта или изменения внешних зависимостей               |
| sec      | Безопасность, уязвимости                                        |
| ci       | Настройка CI и работа со скриптами                              |
| docs	   | Обновление документации                                         |
| feat	   | Добавление нового функционала                                   |
| fix	     | Исправление ошибок                                              |
| perf	   | Изменения направленные на улучшение производительности          |
| refactor | Правки кода без исправления ошибок или добавления новых функций |
| revert   | Откат на предыдущие коммиты                                     |
| style	   | Правки по кодстайлу (табы, отступы, точки, запятые и т.д.)      |
| test	   | Добавление тестов                                               |
| --       | Прочее                                                          |
