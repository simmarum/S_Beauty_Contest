# S_Beauty_Contest
A small project to study to learn distributed processing with MPI library.

## ENGLISH
Pasztetowo decided to end up with harmful stereotypes about the appearance of its residents and residents. In connection with this, the mayor decided to arrange a beauty contest in which visitors from the outside can also compete with residents and residents.

**N** managers spontaneously decide to support a certain number of models this year. Next, the managers apply for access to one of the doctors appointed by the municipality (doctors are **L** and they are distinguishable). Medical examinations are necessary from the time when some jesters put in a competition a powdered pig in a bikini who won the title of vice-miss Pasztetowo. After getting a doctor's opinion, the managers reserve the right number of places in the beauty salon (the salon has a capacity of **S**). Then they report readiness. When all managers are ready, the competition starts (out of program control).

## POLISH
Pasztetowo zdecydowało się raz na zawsze skończyć z krzywdzącymi stereotypami na temat wyglądu swoich mieszkańców i mieszkanek. W związku z tym sołtys postanowił urządzić konkurs piękności, w którym mogą z mieszkańcami i mieszkankami konkurować także przybysze z zewnątrz.

**N** menadżerów spontanicznie decyduje, że będzie w tym roku obsługiwać pewną liczbę modelek. Następnie menadżerowie ubiegają się o dostęp do jednego z lekarzy wyznaczonych przez gminę (lekarzy jest **L** i są rozróżnialni). Badania lekarskie są konieczne od czasu, gdy jacyś dowcipnisie wystawili w konkursie upudrowaną świnię w bikini, która zdobyła tytuł wicemiss Pasztetowa. Po zdobyciu opinii lekarza, menadżerowie rezerwują odpowiednią liczbę miejsc w salonie kosmetycznym (salon ma pojemność **S**). Następnie zgłaszają gotowość. Gdy wszyscy menadżerowie są gotowi, rozpoczyna się konkurs (poza kontrolą programu).

# Require a MPI
```diff
- sudo apt-get install openmpi-bin openmpi-common openmpi-doc libopenmpi-dev openssh-client openssh-server
+ sudo apt-get install libcr-dev mpich mpich-doc
```
If you install **`openmpi`** please remove it (**it not works multithreads aplications...**)  
```bash
sudo apt-get remove openmpi-bin openmpi-common openmpi-doc libopenmpi-dev
```  
And install instead `mpich`  
```bash
sudo apt-get install libcr-dev mpich mpich-doc
```
Verify installation ny execute
```bash
mpicc --version
``` 

# To run program (if no error occurs)
```bash
sh run.sh`
```

# Project editor
Visual Studio Code 
https://code.visualstudio.com/download
