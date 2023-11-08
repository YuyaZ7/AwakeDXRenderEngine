struct BRDFMaterial {
	float baseColor;
	float subsurface;  // sub layer diffuse
	float metallic;	   // looks like metal
	float specular;	   // non metallic part specular
	float specularTint;// specular color looks like base color
	float roughness;
	float anisotropic;	 // asymetric
	float shee;			 // cloth edge bright
	float sheenTint;	 // above looks like base color
	float clearCoat;	 // Another specular term, to simulate varnish
	float clearCoatGloss;// Varnish smoothness
	float padding;
};
