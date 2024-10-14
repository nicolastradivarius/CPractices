/**
 * Los servidores pueden diseñarse de modo que limiten el número de conexiones abiertas. Por ejemplo, un servidor puede autorizar solo N conexiones simultaneas de tipo socket. En cuanto se establezcan las N conexiones, el servidor ya no aceptará otra conexión entrante hasta que una conexión existente se libere. Considere que N = 10, y la cantidad de solicitudes en un determinado momento puede ser mayor que N.
 * 
 * (a) Indique qué mecanismo de comunicación entre procesos considera más apropiada para la resolución de este problema.
 * (b) Implemente el modelo descripto respetando lo indicado en el punto anterior.
 * 
 * Respuesta (a): dado que muchos servidores spawnean procesos o hilos por cada conexión de tipo socket, la mejor opción es tener 
 * 
 * 
 */