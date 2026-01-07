#include <stdio.h>
#include <string.h>
#include <stdlib.h>    // Necesario para malloc y free
#include <RTL.h>
#include <LPC17xx.H>

// ---------------------------------------------------------------------------
// PROTOTIPOS FALTANTES (Parches para compatibilidad de versiones Keil)
// ---------------------------------------------------------------------------
// Definimos manualmente la función de la librería que a veces no sale en el .h
extern void http_get_var (U8 *qstr, U8 *var, U8 *buff, U16 len);

// ---------------------------------------------------------------------------
// VARIABLES EXTERNAS 
// ---------------------------------------------------------------------------
extern float Temperatura;             
extern int umbralTemperatura;         
extern uint32_t contVerde, contNaranja; 
extern char rfid_string_display[13];  
extern int estadoActual;              // 2 = CONFIG, 1 = NORMAL

// ---------------------------------------------------------------------------
// CGI_func: Visualización de Datos
// ---------------------------------------------------------------------------
U16 CGI_func (U8 *env, U8 *buff, U16 buflen, U32 *pcgi) {
  U16 len = 0;

  switch (env[0]) {
    case 't': // Temperatura
      len = sprintf((char *)buff, "%5.1f", Temperatura);
      break;

    case 'u': // Umbral
      len = sprintf((char *)buff, "%d", umbralTemperatura);
      break;

    case 'm': // Modo
      if (estadoActual == 2) { 
          len = sprintf((char *)buff, "<b style='color:orange'>CONFIGURACION</b>");
      } else {
          len = sprintf((char *)buff, "<b style='color:green'>NORMAL</b>");
      }
      break;

    case 'i': // RFID
      len = sprintf((char *)buff, "%s", rfid_string_display);
      break;

    case 'v': // Verdes
      len = sprintf((char *)buff, "%d", contVerde);
      break;

    case 'n': // Naranjas
      len = sprintf((char *)buff, "%d", contNaranja);
      break;
      
    case 'a': // Control condicional del formulario
      // Si estamos en MODO CONFIG (2), mostramos el div (retornamos 1)
      if (estadoActual == 2) {
         len = 1; 
      } else {
         len = 0; 
      }
      break;
  }
  return (len);
}

// ---------------------------------------------------------------------------
// CGI_process_var: Procesamiento de Formularios
// ---------------------------------------------------------------------------
void CGI_process_var (U8 *qs) {
  U8 *var;
  int val;

  // CORRECCIÓN: Usamos malloc estándar en lugar de alloc_mem
  // Esto evita el error de "OS_FRAME undefined"
  var = (U8 *)malloc(40); 
  
  if (var != NULL) {
      // Buscamos la variable "umb"
      // Nota: http_get_var no devuelve true/false en todas las versiones,
      // pero rellena 'var'. Comprobamos si ha escrito algo.
      var[0] = 0; 
      http_get_var (qs, (U8 *)"umb", var, 40);

      // Si la cadena no está vacía, es que encontró el valor
      if (var[0] != 0) {
        val = atoi((const char *)var);
        
        // VALIDACIÓN DE SEGURIDAD
        if (estadoActual == 2) {
            if (val >= 0 && val <= 100) {
                umbralTemperatura = val;
            }
        }
      }
      
      // CORRECCIÓN: Usamos free estándar
      free(var);
  }
}
