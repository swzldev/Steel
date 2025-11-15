func Clamp(float v, float lo, float hi) -> float {
	if (v < lo) {
		return lo;
	} else if (v > hi) {
		return hi;
	} else {
		return v;
	}
}

class ProgressBar {
    constructor(string label, float total) {
        this.label = label;
        this.total = total;
        this.current = 0;
    }

    func Draw() {
        let out = label + " [";
        const completion = current / total;
        const numDots = (completion * 20) as int;
        for (let i = 0; i < 20; i++) {
            if (i < numDots) {
                out = out + "=";
            } else {
                out = out + " ";
            }
        }
        out = out + "]\r";
        Print(out);
    }

    label: string;
    total: float;
    current: float;
}

func Main() -> int {
	const width: int = 80;
    const [int] height = 24;
    const [float] theta_spacing = 0.07;
    const [float] phi_spacing = 0.02;
    const [float] R1 = 1.0;
    const [float] R2 = 2.0;
    const [float] K2 = 40.0;
    const [float] K1 = (width as float) * K2 * 3.0 / (8.0 * (R1 + R2));

    const wxh = width * height;

    let [float] A = 0.0;
    let [float] B = 0.0;

    // Frame loop
    for (let [int] frame = 0; frame < 100; frame = frame + 1) {
        // Output buffer
        let [char[wxh]] output;
        let [float[wxh]] zbuffer;
        for (let [int] i = 0; i < wxh; i = i + 1) {
            output[i] = ' ';
            zbuffer[i] = 0.0;
        }

        const bar = ProgressBar("Rendering", 6.28);

        for (let [float] theta = 0.0; theta < 6.28; theta = theta + theta_spacing) {
            let [float] costheta = Cos(theta);
            let [float] sintheta = Sin(theta);

            for (let [float] phi = 0.0; phi < 6.28; phi = phi + phi_spacing) {
                let [float] cosphi = Cos(phi);
                let [float] sinphi = Sin(phi);

                let [float] circlex = R2 + R1 * costheta;
                let [float] circley = R1 * sintheta;

                // 3D coordinates after rotation
                let [float] x = circlex * (Cos(B) * cosphi + Sin(A) * Sin(B) * sinphi) - circley * Cos(A) * Sin(B);
                let [float] y = circlex * (Sin(B) * cosphi - Sin(A) * Cos(B) * sinphi) + circley * Cos(A) * Cos(B);
                let [float] z = K2 + Cos(A) * circlex * sinphi + circley * Sin(A);
                let [float] ooz = 1.0 / z;

                let [int] xp = (width / 2) + (K1 * ooz * x as int);
                let [int] yp = (height / 2) - (K1 * ooz * y as int);

                let [int] idx = xp + width * yp;
                if (idx >= 0 and idx < wxh and yp >= 0 and yp < height) {
                    if (ooz > zbuffer[idx]) {
                        zbuffer[idx] = ooz;
                        // Luminance calculation
                        let [float] L = cosphi * costheta * Sin(B) - Cos(A) * costheta * sinphi - Sin(A) * sintheta + Cos(B) * (Cos(A) * sintheta - costheta * Sin(A) * sinphi);
                        let [int] luminance_index = (L * 8.0) as int;
                        const luminance = ".,-~:;=!*#$";
                        output[idx] = luminance[Clamp(luminance_index, 0.0, 11.0) as int];
                    }
                }
            }

            bar.current = bar.current + theta_spacing;
            bar.Draw();
        }

        // Convert char array to string for Print
        let outputBuffer = "";
        for (let [int] i = 0; i < width * height; i = i + 1) {
            outputBuffer = outputBuffer + output[i];
            if ((i + 1) % width == 0) {
                outputBuffer = outputBuffer + "\n";
            }
        }

        // Print the frame
        Print(outputBuffer);

        // Animate
        A = A + 0.04;
        B = B + 0.02;

        // Reset cursor
        SetConsolePos(0, 0);
    }

    return 0;
}