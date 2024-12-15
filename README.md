# 6DOF_Robotic_Proyect
Proyecto de brazo robótico basado en biomecánica humana de 6 grados de libertad.

Este repositorio presenta el desarrollo, análisis y validación de un brazo robótico con 6 grados de libertad (GDL). El proyecto incluye aplicación de cinemática directa, cinemática inversa, errores de posicionamiento y su espacio de trabajo, así como las especificaciones mecánicas y componentes utilizados.

---

## 🚀 Objetivo del Proyecto

El objetivo principal de este proyecto fue desarrollar un brazo robótico económico y funcional para aplicaciones académicas y experimentales. El análisis de cinemática directa e inversa y la validación de errores permiten comprender las limitaciones de este tipo de robots.

---

## 🛠️ Diagrama Cinemático del Brazo

![Diagrama Cinemático](Diagrama%20cinematico.png)

El diagrama cinemático muestra la configuración y los ejes de rotación del brazo robótico de 6 GDL. Cada articulación está representada con su respectivo eje \(z\) y los vectores de traslación \(x\), de acuerdo con el modelo matemático utilizado.

---

## 📊 Espacio de Trabajo del Robot

![Espacio de Trabajo del Robot](Espacio%20de%20trabajo.png)

El espacio de trabajo del brazo robótico ha sido determinado utilizando cálculos de cinemática directa.

---

## 📉 Errores en la Cinemática Directa

![Errores Cinemática Directa](Errores%20cinematica%20directa.png)

Los errores en la cinemática directa fueron evaluados al comparar las posiciones objetivo con las posiciones reales. Los errores promedio muestran variaciones significativas debido a la fricción, la inercia y las tolerancias de los materiales de construcción (principalmente PLA).

---

## 📈 Errores en la Cinemática Inversa

![Errores Cinemática Inversa](Error%20Cinematica%20Inversa.png)

En el cálculo de la cinemática inversa, se realizó una secuencia donde el brazo dibuja un cuadrado en el plano. La gráfica muestra los errores de cada servomotor. Los valores reflejan las limitaciones mecánicas y de precisión en el robot, destacando la influencia de la flexión de los materiales y la inercia de los movimientos.

---

## 🔧 Especificaciones Técnicas

- **Peso Total**: <500 g  
- **Carga Máxima**: 100 g  
- **Alcance Máximo**: 30 cm  
- **Velocidad Lineal**: 8 cm/s  
- **Velocidad Angular**: 0.25 rad/s  
- **Torque Mínimo**: >1 Nm  
- **Precisión Angular**: ±10°  
- **Voltaje de Operación**: 5 V  

---

## 📦 Lista de Componentes

| Componente                 | Cantidad |
|----------------------------|----------|
| Servomotores MG996R        | 6        |
| Microcontrolador STM32     | 1        |
| Controlador PCA9685        | 1        |
| Eslabones tipo U           | 6        |
| Eslabones tipo L           | 2        |
| Sargento de 30cm           | 1        |

---

## ✨ Contribuciones

Francisco Javier Reynoso Concepción, Jorge Ignacio Terrazas R.  
Docente: José Jesús Cabrera Pantoja  
