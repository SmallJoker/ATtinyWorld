<Qucs Schematic 2.1.0>
<Properties>
  <View=0,0,1049,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=Charge Pump.dat>
  <DataDisplay=Charge Pump.dpl>
  <OpenDisplay=0>
  <Script=Charge Pump.m>
  <RunScript=0>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
</Symbol>
<Components>
  <C C1 5 550 280 17 -26 0 1 "1 µF" 1 "" 0 "neutral" 0>
  <C C2 5 550 380 17 -26 0 1 "1 µF" 1 "" 0 "neutral" 0>
  <C C3 5 780 280 17 -26 0 1 "10 µF" 1 "" 0 "neutral" 0>
  <Vdc V2 5 400 220 18 -26 0 1 "5 V" 1>
  <GND * 1 400 250 0 0 0 0>
  <GND * 1 400 410 0 0 0 0>
  <Vrect V1 5 400 380 18 -26 0 1 "5 V" 1 "1 ms" 0 "1 ms" 0 "1 ns" 0 "1 ns" 0 "0 ns" 0 "0 V" 1>
  <Diode D_1N4148_4 1 710 430 -26 13 0 0 "222p" 0 "1.65" 0 "4p" 0 "0.333" 0 "0.7" 0 "0.5" 0 "0" 0 "0" 0 "2" 0 "68.6m" 0 "5.76n" 0 "0" 0 "0" 0 "1" 0 "1" 0 "75" 0 "1u" 0 "26.85" 0 "3.0" 0 "1.11" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "26.85" 0 "1.0" 0 "normal" 0>
  <Diode D_1N4148_2 1 710 230 -26 -35 0 2 "222p" 0 "1.65" 0 "4p" 0 "0.333" 0 "0.7" 0 "0.5" 0 "0" 0 "0" 0 "2" 0 "68.6m" 0 "5.76n" 0 "0" 0 "0" 0 "1" 0 "1" 0 "75" 0 "1u" 0 "26.85" 0 "3.0" 0 "1.11" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "26.85" 0 "1.0" 0 "normal" 0>
  <C C4 5 780 380 17 -26 0 1 "10 µF" 1 "" 0 "neutral" 0>
  <GND * 1 720 340 0 0 0 0>
  <Diode D_1N4148_3 1 550 460 13 -26 0 1 "222p" 0 "1.65" 0 "4p" 0 "0.333" 0 "0.7" 0 "0.5" 0 "0" 0 "0" 0 "2" 0 "68.6m" 0 "5.76n" 0 "0" 0 "0" 0 "1" 0 "1" 0 "75" 0 "1u" 0 "26.85" 0 "3.0" 0 "1.11" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "26.85" 0 "1.0" 0 "normal" 0>
  <GND * 1 550 490 0 0 0 0>
  <Diode D_1N4148_1 1 550 200 13 -26 0 1 "222p" 0 "1.65" 0 "4p" 0 "0.333" 0 "0.7" 0 "0.5" 0 "0" 0 "0" 0 "2" 0 "68.6m" 0 "5.76n" 0 "0" 0 "0" 0 "1" 0 "1" 0 "75" 0 "1u" 0 "26.85" 0 "3.0" 0 "1.11" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "26.85" 0 "1.0" 0 "normal" 0>
</Components>
<Wires>
  <550 230 550 250 "" 0 0 0 "">
  <550 410 550 430 "" 0 0 0 "">
  <550 310 550 330 "" 0 0 0 "">
  <550 330 550 350 "" 0 0 0 "">
  <780 310 780 330 "" 0 0 0 "">
  <400 330 400 350 "" 0 0 0 "">
  <400 330 550 330 "" 0 0 0 "">
  <550 430 680 430 "" 0 0 0 "">
  <550 230 680 230 "" 0 0 0 "">
  <780 230 780 250 "" 0 0 0 "">
  <740 230 780 230 "" 0 0 0 "">
  <780 330 780 350 "" 0 0 0 "">
  <740 430 780 430 "" 0 0 0 "">
  <780 410 780 430 "" 0 0 0 "">
  <720 330 780 330 "" 0 0 0 "">
  <720 330 720 340 "" 0 0 0 "">
  <780 430 940 430 "" 0 0 0 "">
  <780 230 940 230 "" 0 0 0 "">
  <400 170 400 190 "" 0 0 0 "">
  <400 170 550 170 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
  <Text 840 200 12 #000000 0 "+8.7 V Output">
  <Text 840 400 12 #000000 0 "-3.7 V Output">
</Paintings>
