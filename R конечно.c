#include <stdio.h>
#include <math.h>

/// Параметры камеры
#define CAM_W       160
#define CAM_H       120
#define CX          (CAM_W / 2.0f)
#define CY          (CAM_H / 2.0f)
#define THETA_MAX   1.3963f             /// 80° в радианах
#define R_MAX       (sqrtf(CX*CX + CY*CY))
#define F           (R_MAX / THETA_MAX) // пикс/рад

///Параметры КА
#define A           0.05f               /// половина стороны кубсата (м)

///
///Преобразование пикселей в углы единой системы КА с учётом конечного расстояния R.
///Вход:
///  x, y – координаты пятна на матрице (целые пиксели)
///  id   – 0: правая камера (ось +45°), 1: левая камера (ось -45°)
///  R    – расстояние от центра КА до источника (м)
///Выход:
///  azimuth   – азимут в общей системе (рад)
///  elevation – угол места (рад)
///

void get_unified_angles_full(int x, int y, int id, float R, float* azimuth, float* elevation) {
    /// Смещения от оптического центра
    float dx = (float)x - CX;
    float dy = (float)y - CY;

    /// Радиальное расстояние (пикс)
    float r = sqrtf(dx*dx + dy*dy);

    /// Зенитный угол θ (отклонение от оптической оси)
    float theta = r / F;

    /// Азимутальный угол φ вокруг оптической оси
    float phi = atan2f(dy, dx);

    /// Горизонтальное и вертикальное отклонение в системе камеры
    float beta1 = theta;   /// горизонтальный угол
    float alpha = asinf(sinf(theta) * sinf(phi));  /// вертикальный угол

    float delta, delta2, eps;
    float delta_v, delta2_v, eps_v;

    /// Горизонтальный угол (азимут)
    if (beta1 > 0) {
        delta = beta1;
    } else {
        delta = -beta1;
    }
    delta2 = asinf((A / R) * sinf(delta));
    eps = delta - delta2;

    if (id == 0) {
        /// Правая камера (ось +45°)
        *azimuth = (float)M_PI_4 + (beta1 > 0 ? eps : -eps);
    } else {
        /// Левая камера (ось -45°)
        *azimuth = -(float)M_PI_4 + (beta1 > 0 ? eps : -eps);
    }

    /// Вертикальный угол
    if (alpha > 0) {
        delta_v = alpha;
    } else {
        delta_v = -alpha;
    }
    delta2_v = asinf((A / R) * sinf(delta_v));
    eps_v = delta_v - delta2_v;
    if (alpha > 0){
        *elevation = eps_v;
    } else{
        *elevation = -eps_v;
    }
}

/// Пример использования
int main(void) {
    int x, y, id;
    float R;
    printf("Введите x y id R(м): ");
    scanf("%d %d %d %f", &x, &y, &id, &R);

    float az, el;
    get_unified_angles_full(x, y, id, R, &az, &el);

    printf("Азимут: %.4f рад (%.2f°)\n", az, az * 180.0f / (float)M_PI);
    printf("Угол места: %.4f рад (%.2f°)\n", el, el * 180.0f / (float)M_PI);
    return 0;
}
