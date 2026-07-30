package mapper

func setBit(mask uint16, pos int, v bool) uint16 {
	if v {
		return mask | (1 << uint(pos))
	}
	return mask &^ (1 << uint(pos))
}

func resizeSlice[T any](s []T, n int, dflt T) []T {
	if n <= len(s) {
		return s[:n]
	}
	extras := make([]T, n-len(s))
	for i := range extras {
		extras[i] = dflt
	}
	return append(s, extras...)
}
