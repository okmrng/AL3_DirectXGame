#pragma once

class Colider {
public:
	// ���a���擾
	float GetRadius() { return radius_; }

	// ���a��ݒ�
	void SetRadius(float radius);

	// �Փˎ��ɌĂ΂��֐�
	virtual void OnColision();

	// ���[���h���W���擾
	virtual Vector3 GetWorldPosition() = 0;

private:
	// �Փ˔���
	float radius_ = 50.0f;
};
