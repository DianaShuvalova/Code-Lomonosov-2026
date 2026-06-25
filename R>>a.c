#include <stdio.h>
#include <math.h>

/// Параметры камеры
#define CAM_W       160
#define CAM_H       120
#define CX          (CAM_W / 2.0f)
#define CY          (CAM_H / 2.0f)
#define THETA_MAX   1.3963f                 /// 80° в радианах
#define R_MAX       (sqrtf(CX*CX + CY*CY))
#define F           (R_MAX / THETA_MAX)     /// пикс/рад

///
///Преобразование пиксельных координат и id камеры в углы единой системы КА.
///Вход:
///     x, y – координаты объекта на матрице (целые пиксели)
///     id   – идентификатор камеры
///Выход:
///     azimuth   – азимутальный угол в горизонтальной плоскости (рад)
///     elevation – угол места (рад)
///
void get_angles_fisheye(int x, int y, int id, float* azimuth, float* elevation) {
    /// Смещения от оптического центра
    float dx = (float)x - CX;
    float dy = (float)y - CY;

    /// Радиальное расстояние
    float r = sqrtf(dx*dx + dy*dy);

    /// Зенитный угол (отклонение от оптической оси)
    float theta = r / F;

    /// Азимутальный угол вокруг оптической оси
    float phi = atan2f(dy, dx);

    /// Вертикальная составляющая (угол места в системе камеры)
    float alpha = asinf(sinf(theta) * sinf(phi));

    /// Горизонтальное отклонение от оси камеры
    float beta1 = theta;

    /// Переход к единой системе КА (приближение R >> a)
    if (id == 0) {
        *azimuth = (float)M_PI_4 + beta1;
    } else {
        *azimuth = -(float)M_PI_4 + beta1;
    }
    *elevation = alpha;
}

int main(void) {
    int x, y, id;
    printf("Введите x y id: ");
    scanf("%d %d %d", &x, &y, &id);

    float az, el;
    get_angles_fisheye(x, y, id, &az, &el);

    printf("Азимут: %.4f рад (%.2f°)\n", az, az * 180.0f / (float)M_PI);
    printf("Угол места: %.4f рад (%.2f°)\n", el, el * 180.0f / (float)M_PI);

    return 0;
}
