﻿#pragma once

#include <iostream>
#include <windows.h>

#include "console.h"
#include "event.h"

/*
* Задача: сделать арканоид на чистых плюсах, как можно лучше
* 
* И так значит, как можно лучше. Первое что приходит в голову, это
* 
* КАРТИНКА=========================================================================================
* Чистые плюсы из коробки имеют только консольную оболочку.
* Ок. Кто сказал что нельзя рисовать в консоли? Проверяем:
* https://docs.microsoft.com/en-us/windows/console/console-functions
* и то ли кто-то продолбился в шары, то ли и впрямь никакого рисования там нет.
* Ладно, консольное окно - тоже окно, значит WinAPI работает для него точно так же.
* wingdi.h в частности. Проверяем, и понимаем что нужен батчинг. Отрисовка квадрата
* 256х256 в течение 2 секунд - это риалтайм уровня первой половины 20ого века.
* Батчим с помощью winuser.h > BeginPaint и понимаем что нихрена не батчится.
* Пачему. Читаем документацию: https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-beginpaint
* Патаму шта винда сама готовит все к батчингу на основе инвалидации конкретных регионов окна.
* Соответственно если нет недействительных регионов, ничего не происходит.
* Пробуем объявить какие-то регионы недействительными самостоятельно, но происходит ровно то,
* что ожидалось; на самом деле может быть что-то еще, но поведение один фиг то же: 
* отправляются WM_ERASEBKGND и/или WM_PAINT, и намертво пришитый к консоли обработчик сообщений
* делает ровно то, для чего был создан - перерисовывает консоль как полагается. Так что когда
* после инвалидации я пытаюсь начать свой батчинг, обработчик сообщений уже все перерисовал
* и мой регион уже аннулирован.
* Патчить готовый обработчик не представляется целесообразным, т.к. это совсем другая работа.
* Пилить свой - тупо впадлу, даже при наличии рабочего шаблона на руках. Даже опуская тот факт,
* что он старый и кривоватый, имея на руках нормальное окно уже сложно отмазаться по-поводу уровня
* графики консолью. И обработчик один хрен надо писать с нуля. ВПАДЛУ. Да и работа эта не быстрая.
* Даже с шаблоном.
* Так что графика в заднице, рисуем ASCII символами. Можно конечно соорудить свой шрифт, но камон.
* Обойдемся тем, что, вроде как, каждую ячейку можно отрисовывать своей парой цветов.
* 
* ФИЗИКА===========================================================================================
* Ага, можно заморочитсья на полноценную симуляцию в вещественном пространстве. Написать SAT или 
* GJK. За пару недель, ибо впервые, ага. Несколько дней портировать выпуклую декомпозицию для 
* сложных случаев, а потом еще пару недель ее допиливать, ибо есть куда. Потом еще выбирать 
* алгоритм предварительного обнаружения коллизии, все равно упереться в AABB-дерево, которое тоже 
* еще ни разу не писалось, ага.
* А потом опупевать от нелинейного поведения квадратов в консоли.
* Так что физика тоже в зданице.
* 
* УПРАВЛЕНИЕ ПАМЯТЬЮ===============================================================================
* Насколько я знаю, плюсы не осуществляют никакого управления памятью во-о-бще, скидывая эту 
* обязанность на систему, которая обязана это уметь для упрощения жизни разработчиков и собственной
* безопасности. Ну и слэб или пул не так сложно накидать. Ток вот смысол в этом здесь какой?
* Страница памяти займет больше чем все приложение со всеми выделнными ресурсами в пике.
* 
* УПРАВЛЕНИЕ ПОТОКАМИ==============================================================================
* Аж целых 1.5 потока, в смысле должно быть 2 - юишный и собственно рабочий, но скорее всего на 1
* тоже все разрулить можно
* 
* СИМУЛЯЦИЯ========================================================================================
* Ну это единственное где есть какой-то смысл что-то выдумывать. И то с ограничениями. Нормальный 
* тикер здесь не упал. В смысле приложение пользовательского кольца имеет доступ к часам с 
* интервалом 15-25 мс. Мне этого должно хватать для полного прогона всей логики несколько раз. Даже
* если я не буду тикать пока система не предоставит мне новые данные с часов, что мне делать с 
* такими крошечными интервалами. Мне м_а_к_с_и_м_у_м есть смысл выдавливать 60 кадров, и то если я
* таки заморочусь на системы частиц, без них 30 кадров будет многовато. Был бы смысл если бы была
* нормальная физическая симуляция, ок, но так и того нету. Так что получается что лучше всего 
* сделать жесткий тикер, выдающий сколько ему сказано. Но, дабы не завязывать всю логику на 
* величину тика, реализовывать все тикающие сущности традиционно - с завязкой чисто на время, а не
* на какие-то ситуативные условности, которые могут исчезать и появляться.
* 
* 
* 
* Оке, поiхали дальше, к самому арканодиу собственно. Устаревшая классика, не вполне совместимая
* с мобильными таймкиллерами, но по сути относящаяся к ним, а значит и требующая то же самое:
* возможность легкого ввода новых механник, подогревающих интерес. То есть слабосвязная
* событийно-ориентированная система. Nuff said.
* 
* Дальше. Реализация, как грится, ни в ***** ни в красную армию. Жалко руинить потанцевально
* допиливаемую вещь. Главная руина - графическая составляющая. Отвязываем ее.
*/

void kekw(int a, void* b) {
    std::cout << "kekw " << a << " " << b;
}

int main(int argc, const char** argv)
{
    //std::cout << "Hello World!\n";

    //HWND consoleHandle = GetConsoleWindow();
    //HDC contextHandle = GetDC(consoleHandle);
    //SetConsoleMode(consoleHandle, ENABLE_EXTENDED_FLAGS);

    ////_SECURITY_ATTRIBUTES secAttr{sizeof(secAttr),NULL, false};
    ////HANDLE newScreenBuffer = CreateConsoleScreenBuffer(
    ////    GENERIC_READ | GENERIC_WRITE,
    ////    0,
    ////    NULL,
    ////    CONSOLE_TEXTMODE_BUFFER,
    ////    NULL
    ////);

    //SetConsoleCursorVisibility(false);
    //PAINTSTRUCT dummy;
    //RECT rect{ 0,0,255,255 };
    //InvalidateRect(consoleHandle, &rect, true);
    //auto drawingContext = BeginPaint(consoleHandle, &dummy);
    //for (auto x = 0; x < 256; x++)
    //    for(auto y = 0; y < 256; y++)
    //        SetPixel(drawingContext, x, y, RGB(255, 255, 255));
    //EndPaint(consoleHandle, &dummy);
    ////SetConsoleActiveScreenBuffer(newScreenBuffer);

    //ReleaseDC(consoleHandle, contextHandle);

    //EventBus<int, int, void*> eb;
    //eb.subscribe(0, &kekw);
    //eb.fireEvent(123, (void*)456);

    std::cin.ignore();
    return 0;
}