#pragma once

class Colider {
public:
	// ”¼Œa‚ğæ“¾
	float GetRadius() { return radius_; }

	// ”¼Œa‚ğİ’è
	void SetRadius(float radius);

	// Õ“Ë‚ÉŒÄ‚Î‚ê‚éŠÖ”
	virtual void OnColision();

	// ƒ[ƒ‹ƒhÀ•W‚ğæ“¾
	virtual Vector3 GetWorldPosition() = 0;

private:
	// Õ“Ë”»’è
	float radius_ = 50.0f;
};
