# Inverted_Pendulum
Stabilised inverted pendulum on a cart with  a LQR

![aaaaaa](https://github.com/BosieIonut/Self_Balaning_Robot_LQR/assets/33691449/8fbdd663-e495-4439-977b-1bf3e2deb2e0)


<h1><b> Pendul invers </h1></b>


<img src = "https://i.imgur.com/cLhNwkR.png" height ="350"><br>

Pentru a realiza acest proiect am folosit urmatoarele componente:
<ul>
<li> Arduino Mega 2560
<li> MPU6050 acceelerometru si giroscop
<li> L298N Dual Motor Driver
<li> 3 baterii de litiu de 1200 mAh 3.7V (dar in realitate au mai mult)
<li> potentiometru
<li> buton on/off
<li> fire
<li> cadru facut manual
</ul>



<h2><b> Citirea datelor si filtrarea lor </h2></b>
<img src = "https://i.ebayimg.com/images/g/K20AAOSw3KFWfM7i/s-l500.jpg" width ="70"><br>


Pentru a citi datele de la acceelerometru folosim protocolul I2C. <br>
Folosindu-ma de acceelerometru pot calcula destul de precis valoarea unghiului in functie de orientarea greutatii ( aceasta metoda este buna la 'frecvente joase', daca robotelul este in miscare sau, vibreaza, vom obtine rezultate proaste).<br>
Pe de alta parte ma pot folosi de giroscop pentru a integra viteza unghiulara obinand o valoare precisa la frecvente inalte, dar in timp senzorul fiind afectat de noise, eroarea se va acumula si unghiul va devia.<br>
<br>
Solutia este sa folosim un filtru complementar care se foloseste de datele accelerometrului la frecvente joase si de datele giroscopului la frecvente inalte. In felul acesta vom obtine o valoare a unghiului destul de precisa pentru a putea stabiliza robotul.
<br>
<center><img src = "https://i.imgur.com/2m3d1Qp.png" width ="70%"><br></center>


<h2> Comanda si calcularea acesteia<h2>

Comanda motoarelor se va da puntii H prin 2 pini de pulse width modulation.<br>
VCC-ul motoarelor este de 12V, deci intervalul 0-255 (dat pe pinul de pwm) se va mapa in 0-12V.

Comanda se va calcula folosin un regulator PID pentru care am setat setat valorile constantelor KI,KD,KP experimental.

In plus, pentru ca perioada de esantionare este mica 0.005s, voi aplica componentei derivative si un filtru de tip Chebyshev2 (calculat in matlab), acesta ne va ajuta sa reducem vibratiile.

O alta problema este ca, din cauza alinierii componentelor, centrul de greutate nu se afla aliniat cu centrul geometric si in consecinta robotul se va inclina intr-o parte. Pentru a rezolva aceasta problema am pus un potentiometru care modifica valoarea SetPoint-ului.



<h2>Circuitul complet </h2>
<img src="https://i.imgur.com/lTyNOnd.png">


<h2> Exemplu de simulare grafica </h2>

In figura de mai jos au fost plotate comanda, eroarea si unghiul in momentul in care se aplica o perturbatie sistemului

<img src="https://i.imgur.com/mFkkfph.png">
