import type {EqBandState} from "./api_data.ts";
import {Component, createRef} from "preact";
import {useEffect} from "preact/hooks";

type EqProps = {
  bands: EqBandState[];
  width: number;
  height: number;
  className?: string;
  onNewUserVal?: (value: number) => Promise<void>;
  syncBackAfter?: number;
  syncBackAfterDuringDrag?: number;
};

class Biquad {
  public b0: number;
  public b1: number;
  public b2: number;
  public a0: number;
  public a1: number;
  public a2: number;

  constructor() {
    this.b0 = 0;
    this.b1 = 0;
    this.b2 = 0;
    this.a0 = 0;
    this.a1 = 0;
    this.a2 = 0;
  }

  normalize() {
    this.b0 /= this.a0;
    this.b1 /= this.a0;
    this.b2 /= this.a0;
    this.a1 /= this.a0;
    this.a2 /= this.a0;
    this.a0 = 1.0;
  }

  calculate(f: number) {
    // Don't worry about it
    const phi = (Math.sin(Math.PI * f * 2 / (2 * 48000))) ** 2;
    let r = ((this.b0 + this.b1 + this.b2) ** 2 - 4 * (this.b0 * this.b1 + 4 * this.b0 * this.b2 + this.b1 * this.b2) * phi + 16 * this.b0 * this.b2 * phi * phi) / ((1 + this.a1 + this.a2) ** 2 - 4 * (this.a1 + 4 * this.a2 + this.a1 * this.a2) * phi + 16 * this.a2 * phi * phi);
    if (r < 0) {
      r = 0;
    }
    r = r ** 0.5
    try {
      return 20 * Math.log10(r);
    } catch {
      return -200;
    }
  }
}

function biquadHighpass(f: number, q: number) {
  const w0 = (2 * Math.PI * f) / 48000;
  const s0 = Math.sin(w0);
  const c0 = Math.cos(w0);
  const alpha = s0 / (2 * q);

  const self = new Biquad();
  self.b0 = (1 + c0) / 2
  self.b1 = -(1 + c0)
  self.b2 = (1 + c0) / 2
  self.a0 = 1 + alpha
  self.a1 = -2 * c0
  self.a2 = 1 - alpha
  self.normalize()
  return self
}

function biquadLowpass(f: number, q: number) {
  const w0 = (2 * Math.PI * f) / 48000;
  const s0 = Math.sin(w0);
  const c0 = Math.cos(w0);
  const alpha = s0 / (2 * q);

  const self = new Biquad();
  self.b0 = (1 - c0) / 2
  self.b1 = 1 - c0
  self.b2 = (1 - c0) / 2
  self.a0 = 1 + alpha
  self.a1 = -2 * c0
  self.a2 = 1 - alpha
  self.normalize()
  return self;
}

function biquadPeak(f: number, gain: number, q: number) {
  const A = Math.pow(10, gain / 40);
  const w0 = (2 * Math.PI * f) / 48000;
  const s0 = Math.sin(w0);
  if (q === 0) {
    q = 1;
  }
  const alpha = s0 / (2 * q);

  const self = new Biquad();
  self.b0 = 1 + alpha * A;
  self.b1 = -2 * Math.cos(w0);
  self.b2 = 1 - alpha * A;
  self.a0 = 1 + alpha / A;
  self.a1 = -2 * Math.cos(w0);
  self.a2 = 1 - alpha / A;
  self.normalize();
  return self;
}

function biquadNotch(f: number) {
  const q = 7;
  const w0 = (2 * Math.PI * f) / 48000;
  const s0 = Math.sin(w0);
  const alpha = s0 / (2 * q);

  const self = new Biquad();
  self.b0 = 1
  self.b1 = -2 * Math.cos(w0)
  self.b2 = 1
  self.a0 = 1 + alpha
  self.a1 = -2 * Math.cos(w0)
  self.a2 = 1 - alpha;
  self.normalize();
  return self;
}

function biquadLowshelf(f: number, gain: number) {
  const A = Math.pow(10, gain / 40);
  const w0 = (2 * Math.PI * f) / 48000;
  const s0 = Math.sin(w0);
  const c0 = Math.cos(w0);
  const beta = Math.sqrt(A + A);

  const self = new Biquad();
  self.b0 = A * ((A + 1) - (A - 1) * c0 + beta * s0);
  self.b1 = 2 * A * ((A - 1) - (A + 1) * c0);
  self.b2 = A * ((A + 1) - (A - 1) * c0 - beta * s0);
  self.a0 = (A + 1) + (A - 1) * c0 + beta * s0;
  self.a1 = -2 * ((A - 1) + (A + 1) * c0);
  self.a2 = (A + 1) + (A - 1) * c0 - beta * s0;
  self.normalize()
  return self
}

function biquadHighshelf(f: number, gain: number) {
  const A = Math.pow(10, gain / 40);
  const w0 = (2 * Math.PI * f) / 48000;
  const s0 = Math.sin(w0);
  const c0 = Math.cos(w0);
  const beta = Math.sqrt(A + A);

  const self = new Biquad();
  self.b0 = A * ((A + 1) + (A - 1) * c0 + beta * s0);
  self.b1 = -2 * A * ((A - 1) + (A + 1) * c0);
  self.b2 = A * ((A + 1) + (A - 1) * c0 - beta * s0);
  self.a0 = (A + 1) - (A - 1) * c0 + beta * s0;
  self.a1 = 2 * ((A - 1) - (A + 1) * c0);
  self.a2 = (A + 1) - (A - 1) * c0 - beta * s0;
  self.normalize()
  return self
}


export class EqCurve extends Component<EqProps, any> {
  private biquads: Biquad[] = [];
  private canvasRef = createRef<HTMLCanvasElement>();

  constructor(props: EqProps) {
    super(props);
  }

  render() {

    useEffect(() => {
      if (this.canvasRef.current) {
        this.renderFilters(this.canvasRef.current);
      }
    }, [this.props.bands]);

    return (
        <canvas
            ref={this.canvasRef}
            width={this.props.width}
            height={this.props.height}
            className={["eq-curve", this.props.className]
                .filter(Boolean)
                .join(" ")}
        />
    );
  }

  x_to_f(x: number, canvas: HTMLCanvasElement) {
    const frac = (x + 1) / (canvas.width + 1);
    const log_min = Math.log2(19);
    const log_max = Math.log2(19000);
    const log_delta = log_max - log_min;
    return 2 ** (log_min + (log_delta * frac));
  }

  db_to_y(dB: number, canvas: HTMLCanvasElement) {
    const fract = dB / 20;
    return canvas.height - (fract * (canvas.height / 2) + canvas.height / 2);
  }

  calc_filter(f: number) {
    let dB = 0.0;
    for (let b = 0; b < this.biquads.length; b++) {
      dB += this.biquads[b].calculate(f);
    }
    return dB;
  }

  renderBiquads(canvas: HTMLCanvasElement) {
    const ctx = canvas.getContext("2d");

    if (ctx === null) {
      return;
    }

    ctx.fillStyle = "#333";
    ctx.strokeStyle = "#888";
    ctx.fillRect(0, 0, canvas.width, canvas.height);

    ctx.beginPath();
    ctx.moveTo(0, canvas.height / 2);
    ctx.lineTo(canvas.width, canvas.height / 2);
    ctx.stroke();

    ctx.strokeStyle = "#09F";
    ctx.beginPath();
    const last = this.db_to_y(this.calc_filter(19), canvas);
    ctx.moveTo(0, last);
    for (let fP = 0; fP < canvas.width; fP++) {
      const f = this.x_to_f(fP, canvas);
      const dB = this.calc_filter(f);
      const gain = this.db_to_y(dB, canvas);
      ctx.lineTo(fP, gain);
    }
    ctx.stroke();
  }

  renderFilters(canvas: HTMLCanvasElement) {
    this.biquads = [];
    for (let i = 0; i < this.props.bands.length; i++) {
      const band = this.props.bands[i];
      switch (band.type) {
        case 2:
          this.biquads.push(biquadHighpass(band.frequency, band.q));
          break;
        case 1:
          this.biquads.push(biquadLowpass(band.frequency, band.q));
          break;
        case 6:
          this.biquads.push(biquadPeak(band.frequency, band.gain, band.q));
          break;
        case 5:
          this.biquads.push(biquadNotch(band.frequency));
          break;
        case 3:
          this.biquads.push(biquadLowshelf(band.frequency, band.gain));
          break;
        case 4:
          this.biquads.push(biquadHighshelf(band.frequency, band.gain));
          break;
      }
    }
    this.renderBiquads(canvas);
  }

}