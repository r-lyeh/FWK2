struct Quaternion {
    double w, x, y, z;
};

struct EulerAngles {
    double roll, pitch, yaw;
};

#if 0
double deg2rad(double deg) {
    return deg * M_PI / 180.0;
}

double rad2deg(double rad) {
    return rad * 180.0 / M_PI;
}

float quat_pitch(quat q) {
    return atan2f(2.0f * q.y * q.z + q.w * q.x, q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z);
}

float quat_yaw(quat q) {
    return asinf(-2.0f * (q.x * q.z - q.w * q.y));
}

float quat_roll(quat q) {
    return atan2f(2.0f * q.x * q.y +  q.z * q.w,  q.x * q.x + q.w * q.w - q.y * q.y - q.z * q.z);
}
#endif

// euler <-> quat
vec3  euler    (quat q_) { // returns degrees. ref: https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
    struct Quaternion q = { q_.x, q_.y, q_.z, q_.w };
    struct EulerAngles angles;

    // roll (x-axis rotation)
    double sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
    double cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
    angles.roll = atan2f(sinr_cosp, cosr_cosp);

    // pitch (y-axis rotation)
    double sinp = sqrtf(1 + 2 * (q.w * q.y - q.x * q.z));
    double cosp = sqrtf(1 - 2 * (q.w * q.y - q.x * q.z));
    angles.pitch = 2 * atan2f(sinp, cosp) - M_PI / 2;

    // yaw (z-axis rotation)
    double siny_cosp = 2 * (q.w * q.z + q.x * q.y);
    double cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
    angles.yaw = atan2f(siny_cosp, cosy_cosp);

    return vec3(angles.roll, angles.pitch, angles.yaw);
}

quat  eulerq   (vec3 degrees) {
    float roll = degrees.x, pitch = degrees.y, yaw = degrees.z;

    // Abbreviations for the various angular functions

    double cr = cosf(roll * 0.5);
    double sr = sinf(roll * 0.5);
    double cp = cosf(pitch * 0.5);
    double sp = sinf(pitch * 0.5);
    double cy = cosf(yaw * 0.5);
    double sy = sinf(yaw * 0.5);

    struct Quaternion q;
    q.w = cr * cp * cy + sr * sp * sy;
    q.x = sr * cp * cy - cr * sp * sy;
    q.y = cr * sp * cy + sr * cp * sy;
    q.z = cr * cp * sy - sr * sp * cy;

    return quat(q.w,q.x,q.y,q.z);
}