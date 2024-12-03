#include <Arduino.h>
#include <lvgl.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

// Buffer de dibujo
#define DRAW_BUF_SIZE (TFT_HEIGHT * TFT_WIDTH / 10 * (LV_COLOR_DEPTH / 8))
uint32_t draw_buf[DRAW_BUF_SIZE / 4];

// Función de tick para LVGL
static uint32_t my_tick(void)
{
    return millis();
}

// Objetos gráficos
lv_obj_t *graph;
lv_chart_series_t *temp_series;
lv_obj_t *temp_label;
lv_obj_t *label;  // Añadido para el primer label

#define GRAPH_HEIGHT 120
#define GRAPH_POINTS 10
int temperature_values[GRAPH_POINTS] = {0}; // Array para los valores de temperatura

void setup()
{
    Serial.begin(115200); // Asegúrate de que la velocidad del puerto serial es la correcta

    // Inicializar LVGL
    lv_init();
    lv_tick_set_cb(my_tick);
    lv_display_t *disp = lv_tft_espi_create(TFT_WIDTH, TFT_HEIGHT, draw_buf, sizeof(draw_buf));

    // Establecer el fondo en gris claro
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_make(0, 0, 0), 0);

    // Crear la etiqueta de saludo
    label = lv_label_create(lv_scr_act()); // Usar lv_scr_act() para la pantalla activa
    lv_label_set_text(label, "Temperatura en tiempo real");
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 10); // Alineación mejorada

    // Cambiar el color del texto del primer label a blanco
    lv_obj_set_style_text_color(label, lv_color_make(255, 255, 255), 0);

    // Crear la gráfica para la temperatura
    graph = lv_chart_create(lv_scr_act());
    lv_obj_set_size(graph, TFT_WIDTH, GRAPH_HEIGHT); // Gráfica al ancho de la pantalla
    lv_obj_align(graph, LV_ALIGN_TOP_LEFT, 0, 40);   // Colocar debajo de la etiqueta

    // Configuración de la gráfica
    lv_chart_set_type(graph, LV_CHART_TYPE_LINE);               // Tipo de gráfica de línea
    lv_chart_set_point_count(graph, GRAPH_POINTS);              // Número de puntos a mostrar
    lv_chart_set_range(graph, LV_CHART_AXIS_PRIMARY_Y, 0, 100); // Rango de la gráfica (0 a 100 ºC)

    // Crear la serie para la temperatura (sin especificar color)
    temp_series = lv_chart_add_series(graph, lv_color_make(255, 50, 0), LV_CHART_AXIS_PRIMARY_Y);

    temp_label = lv_label_create(lv_scr_act());
    lv_label_set_text(temp_label, "TEMP: xx°C"); 
    lv_obj_set_pos(temp_label, 85,170); 
    lv_obj_set_style_text_color(temp_label, lv_color_make(255, 255, 255), 0);
}

void loop()
{
    lv_timer_handler(); // Actualizar LVGL

    // Si hay datos disponibles en el puerto serial
    if (Serial.available() > 0)
    {
        String received = Serial.readStringUntil('\n'); // Leer hasta encontrar una nueva línea
        received.trim();                                // Eliminar espacios innecesarios

        // Convertir la cadena recibida en un valor entero
        int temperature = received.toInt(); // Convertir la cadena a entero (temperatura en ºC)

        // Asegurarse de que la temperatura esté dentro del rango (0 a 100 ºC)
        if (temperature < 0)
            temperature = 0;
        if (temperature > 100)
            temperature = 100;

        for (int i = 0; i < GRAPH_POINTS - 1; i++)
        {
            temperature_values[i] = temperature_values[i + 1]; 
        }
        temperature_values[GRAPH_POINTS - 1] = temperature; 

        for (int i = 0; i < GRAPH_POINTS; i++)
        {
            lv_chart_set_next_value(graph, temp_series, temperature_values[i]); 
        }

        lv_label_set_text_fmt(temp_label, "TEMP: %d°C", temperature, 175); 
    }

    delay(100); 
}
