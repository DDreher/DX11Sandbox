
/**
 * Transform consisting of scale, rotation (quaternion) and translation.
 */
class Transform
{
public:
    Mat4 ToMatrix()
    {
        return Mat4::Scaling(scaling_) * rotation_.ToMatrix() * Mat4::Translation(translation_);
    }

    inline Quat GetRotation() const
    {
        return rotation_;
    }

    float GetPitch() const;
    float GetYaw() const;
    float GetRoll() const;

    Vec3 scaling_;
    Quat rotation_;
    Vec3 translation_;

private:

};
