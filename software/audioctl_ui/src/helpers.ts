export function formatDb(n: number): string {
  const sign = n < 0 ? '-' : '\u00A0'
  const abs = Math.abs(n)
  const intPart = String(Math.floor(abs)).padStart(3, '0')
  const decPart = Math.floor((abs % 1) * 10)
  return `${sign}${intPart}.${decPart} dB`
}

export function logLinear(db: number): number {
  return 0.79306 * Math.exp(db * 0.0527087)
}