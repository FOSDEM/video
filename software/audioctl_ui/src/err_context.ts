import { createContext } from "preact"

import type { ErrHandler } from "rtui"

export const ErrHandlerCtx = createContext<ErrHandler>(dummy_err_handler)

function dummy_err_handler(err: any) {
  console.error("UI action failed: ", err)
}
