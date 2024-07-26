### Theoretischen Teil :

TLBs (kleine, schnelle Caches) sind Teil der Speichermanagementeinheit (MMU), die die neuesten Übersetzungen von virtuellen Speicher- zu physischen Speicheradressen speichert, um den Zugriff zu beschleunigen.

In ihrer Studie "A Simulation Based Study of TLB Performance" simulierten J. Bradley Chen, Anita Borg und Norman P. Jouppi TLBs mit Größen von 8 bis 64 Einträgen und Seitengrößen von 4K bis 64K Byte, wobei sie eine 100-Zyklen-Strafe für TLB-Misses verwendeten. Ihre Forschung untersuchte verschiedene TLB-Architekturen, wie unabhängige TLBs für Instruktions- und Daten-Caches sowie zweistufige TLB-Architekturen (Micro-TLB) für Datenreferenzen. Für unsere Simulation konzentrieren wir uns ausschließlich auf das Instruction TLB. Ihre Experimente legen nahe, dass ein Instruction TLB mit 16 Einträgen und 16K-Byte-Seiten für die meisten Anwendungs-Workloads ausreicht.

## Übliche TLB-Größen und Architekturen

Moderne Prozessoren verwenden typischerweise TLBs mit 32 bis 512 Einträgen. Sie sind hierarchisch aufgebaut, bestehend aus L1- und L2-TLBs.

## Latenzen

- Hauptspeicherlatenz: 50-100 ns
- L1-TLB-Latenz: 1-3 Zyklen
- L2-TLB-Latenz: 7-10 Zyklen

## Alle in der TLB verwendeten Module und ihre primitive Gatteranzahl:

AND*GATE: (2) * 1 Gatter
COMPARATOR: (1) _ 63 Gatter
DEMULTIPLEXER: (1) _ (log2(TLB*SIZE) + 2 * TLB*SIZE) Gatter
D_FLIP_FLOPs: (TLB_LINE_LENGTH * TLB*SIZE) * 4 Gatter
MULTIPLEXER: (1) _ (log2(TLB_SIZE) + 64TLB_SIZE - 32) Gatter
FINAL_MUX: (1) _ 97 Gatter
NOT*GATE: (1) * 1 Gatter
Unsere Formel: (131 + 2 * log2(TLB*SIZE) + 70 \* TLB_SIZE + 4 \* TLB_LINE_LENGTH) Gatter

## Ergebnisse

Die folgende Tabelle zeigt die Hits und Misses in Abhängigkeit von der TLB-Größe in einer Summe über eine einfach verketete Liste für eine blocksize von 32:

| TLB-Größe | Misses | Hits | Cycles |
| --------- | ------ | ---- | ------ |
| 8         | 30     | 14   | 7488   |
| 16        | 24     | 20   | 6888   |
| 32        | 18     | 26   | 6288   |
| 64        | 16     | 28   | 6088   |

### Persönliche Beiträge jedes Gruppenmitglieds

### Safa El Benna :
Ich habe die Dateien erstellt und das dflip flop Modul hinzugefügt (die Implementierung habe ich aus der Übungsaufgabe T5 übernommen)

Ich habe ein Makefile hinzugefügt, das stark vom Makefile aus T8 inspiriert ist. Außerdem habe ich eine erste Version des Rahmenprogramms in der Datei main.c implementiert, die in C geschrieben ist. Zur Analyse der Eingabeoptionen habe ich getopt_long verwendet, gemäß den Anforderungen der Aufgabe. Die Analyse ist etwas vom Nutzereingaben-Tutorial inspiriert, obwohl sie nicht die gleiche Funktion verwendet. Diese Implementierung konvertiert die Eingabestrings in ihre entsprechenden Werte, behandelt Fehler und gibt entsprechende Fehlermeldungen an stderr aus.

Anschließend analysiere ich die Eingabedatei, um sicherzustellen, dass sie das richtige Format hat. Zunächst zähle ich die Anzahl der Zeilen, um die Anzahl der Anfragen zu ermitteln, und verarbeite dann jede Zeile einzeln, um die Werte zu extrahieren. Wenn eine Zeile nicht den erforderlichen Typen und dem Format entspricht, wird ein entsprechender Fehler gemeldet. Ich habe auch einige Standardwerte festgelegt, die derzeit keine große Bedeutung haben, aber in Zukunft an Industriestandards angepasst werden sollten.

Abschließend habe ich die Methode run_simulation mit den angegebenen Argumenten aufgerufen und ihr Ergebnis einer Result-Struktur zugewiesen. Für simulation.cpp habe ich nur das Layout der Tracefile hinzugefügt. Diese muss später im Entwicklungsprozess mit bestimmten Signalen verbunden werden.

Ich habe die Datei TLB_Line.hpp erstellt, um eine Zeile im TLB darzustellen. Das Hauptziel war es, nur die wesentlichen Komponenten der Adresse zu speichern. Dazu entfernen wir den Offset und den TLB-Index aus der virtuellen Adresse und verknüpfen den verbleibenden Teil mit der physischen Adresse. Jedes TLB_Line Objekt enthält außerdem ein validBit, um anzuzeigen, ob es gültige Werte enthält.

Ich habe die erste Version der TLB-Modul fertiggestellt, das für die Integration aller anderen Module verantwortlich ist, einschließlich TLB_LINE, DEMULTIPLEXER, LOOKUP_TABLE, MULTIPLEXER, COMPARATOR und SIMPLE_MUX.Dieses Modul wurde auf Grundlage unserer Logisim-Simulation umgesetzt.

Ich habe die wichtigsten Signale mit der Tracefile verbunden.Und Die clean Funktionalität der Makefile angepasst, um es zu entfernen,wenn wir wollen.

### Bobur Khayitov :

Beitrag zur Projektarbeit

Im Rahmen des GRA-Projekts habe ich mich vor allem auf die Bereiche DevOps und Code-Kontrolle konzentriert. Hier sind die spezifischen Aufgaben und Verantwortlichkeiten, die ich übernommen habe:

DevOps und Git-Management

Ich habe mein in der Industrie erworbenes Wissen genutzt, um die Git-Repository-Organisation zu optimieren. Dies umfasste die Verwaltung aller Git-Aktionen wie Änderungen, das Zurücksetzen auf frühere Commits, Rebasen und das Vergleichen von Unterschieden. Mein Ziel war es, einen reibungslosen Workflow für das Team zu gewährleisten.

Code-Kontrolle

Ein weiterer wichtiger Aspekt meiner Arbeit war die Kontrolle des Codes. Ich achtete darauf, dass wir nicht zu viel Boilerplate-Code haben und vermied unverständliche Namensgebungen. Darüber hinaus fügte ich notwendige Kommentare hinzu, um den Code verständlicher und wartbarer zu machen.

Entwicklung von Modulen

Ich habe zwei entscheidende Konzepte entwickelt, die uns viel Zeit erspart und Bugs vermieden haben. Tester-Modul wird für Unit-Tests verwendet. Bei diesem Modul haben wir jedes Modul einzeln auf seine Ein- und Ausgänge getestet, um deren ordnungsgemäße Funktion zu überprüfen und deren Verhalten zu simulieren. Globale Konstanten(und Variablen) haben uns enorm viel Zeit und Code gespart. Sie wird in einem Header definiert und in alle notwendigen Dateien aufgenommen, die darauf zugreifen. Es handelt sich um eine einzige Quelle der Wahrheit, die das Risiko von Datenverlusten und Dateninkonsistenzen verringert. 

Zusammenarbeit im Team

Ich habe an der Erstellung der meisten Hauptmodule mitgewirkt, indem ich Ideen eingebracht und diese gemeinsam mit meinen Teamkollegen implementiert habe. Unsere Arbeitsweise war stark teamorientiert; wir haben die meisten Module gemeinsam entwickelt und darauf geachtet, dass alle Beteiligten in den Entwicklungsprozess einbezogen wurden.

### Mohsen Cheikh Rouhou :

Mein Beitrag zum Projekt war zu Beginn die theoretische Arbeit und das Verständnis des TLB für mich und die Gruppenmitglieder. Wir begannen mit der Recherche und dem Austausch unseres Wissens, um diese neu vorgestellte Hardwarekomponente zu verstehen und sicherzustellen, dass unsere Simulation relevant ist.

Ich habe einen Versuch unternommen, den TLB zu entwerfen und ihn in Logisim Evolution zu visualisieren, um die Wahl der Module und die Signalkopplung zu erleichtern und stärker an einem Hardware-Ansatz festzuhalten.

Ich habe das Hauptspeichermodul implementiert, das das Verhalten des Hauptspeichers mithilfe einer verknüpften Liste als Abstraktion nachahmt. Wir haben zu Beginn separate Module (Hardwarekomponenten) implementiert und versucht, sie zu verknüpfen. Dann haben wir schließlich versucht, jeder in seinem eigenen Zweig zu arbeiten, aufgrund der Komplexität und Schwierigkeit, das Hauptmodul (TLB.hpp) zu erstellen.

In meiner Version habe ich versucht, einen Scheduling-Mechanismus für die Threads zu verwenden, um den Signalfluss im System mit sc_event als sensitiven Verhalten zu steuern, was von unserem ursprünglichen Ansatz abweichen könnte, es so nah wie möglich an einem hardwareähnlichen TLB zu gestalten.

Zusätzlich hatte ich einen Teil bei der Erstellung der Makefile, um den Build-Prozess unseres Projekts zu automatisieren und zu vereinfachen.

Ich habe auch die Präsentationsfolien erstellt, da es einfacher ist, die Implementierung mit dem Logisim-Evolution-Design zu erklären.

Während der Implementierungsphase nutzte ich den GDC-Debugger von C++ für die Fehlerbehebung und das Testen der Module. Dies half uns, Fehler schnell zu identifizieren und zu beheben. Ich versuchte, eine Datenbank mit realen CPU-Traces für eine 32-Bit-Architektur zu finden, um Benchmark-Tests durchzuführen und die Latenzen in realen Szenarien zu analysieren.

Leider war es schwierig, solch alte und spezifische Daten zu finden, da sie meist kostenpflichtig sind. Zusätzlich verglich ich die Ergebnisse unserer TLB-Simulation mit vorhandenen Forschungsergebnissen, um die Übereinstimmung der Verhaltensmuster zu überprüfen.