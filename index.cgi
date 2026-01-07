t <html><head><title>SEDA Final</title>
t <meta http-equiv="refresh" content="20">
t <style>
t body { font-family: Verdana; background-color: #f4f4f4;
t        text-align: center; }
t .panel { display: inline-block; background: white;
t          padding: 20px; border-radius: 10px;
t          box-shadow: 0 0 10px rgba(0,0,0,0.1);
t          text-align: left; width: 400px;}
t h2 { color: #003366;
t border-bottom: 2px solid #003366; }
t .dato { font-size: 18px; margin: 10px 0;
t         border-bottom: 1px dotted #ccc; padding: 5px; }
t .val { font-weight: bold;
t color: #0066cc; float: right; }
t .g { color: green; } .r { color: red; } .o { color: orange;
t }
t input[type=text] { width: 50px; text-align: center; }
t </style>
t </head>
t <body>
t <div class="panel">
t <h2>Monitor SEDA (LPC1768)</h2>
t <div class="dato">Estado: <span class="val">
c m
t </span></div>
t <div class="dato">Temp: <span class="val">
c t
t  C</span></div>
t <div class="dato">Umbral: <span class="val">
c u
t  C</span></div>
t <div class="dato">RFID: <span class="val" style="color:black;">
c i
t </span></div>
t <h3>Contadores</h3>
t <table width="100%">
t <tr><td align="center"><b class="g">VERDE</b></td>
t <td align="center"><b class="o">NARANJA</b></td></tr>
t <tr><td align="center"><h1>
c v
t </h1></td>
t <td align="center"><h1>
c n
t </h1></td></tr>
t </table>
c a 1 <div style="background-color:#ffc; padding:10px; border:1px solid orange;">
t <b>[CONFIG]</b><br>
t <form action="index.cgi" method="GET">
t Umbral: <input type="text" name="umb" value="
c u
t ">
t <input type="submit" value="Enviar">
t </form>
c a 2 </div>
t </div>
t </body></html>
.
