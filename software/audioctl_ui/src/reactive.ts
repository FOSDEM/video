import {
  useRef,
  useEffect,
  useState,
  useContext,
  type Dispatch,
  type StateUpdater,
} from "preact/hooks"
import { Throttler } from "rtui"
import { ErrHandlerCtx } from "./err_context.ts"

export default function useReactive<T>(opts: {
  real_val: T
  on_new_user_val: (v: T) => Promise<void>
  sync_back_after?: number
}): [T, Dispatch<StateUpdater<T>>] {
  const throttler = useRef(new Throttler())
  const err_handler = useContext(ErrHandlerCtx)

  const { real_val, on_new_user_val, sync_back_after } = opts
  const [user_val, set_user_val] = useState(real_val) // initial
  const [user_mirrors_real, set_user_mirrors_real] = useState(false)

  // on "user_val" value change, enqueue to qui
  useEffect(() => {
    // TODO: this effect MUST NOT fire when caused by sync_user_to_real
    console.log(`user val change to ${user_val}`)

    throttler.current.do(
      async () => {
        set_user_mirrors_real(false)
        console.log(`user val begin update to ${user_val}`)
        await on_new_user_val(user_val)
        console.log(`user val end update to ${user_val}`)
      },
      {
        sync_back: () => set_user_mirrors_real(true),
        sync_back_after: sync_back_after,
        err_handler: err_handler,
      },
    )
  }, [user_val])

  const sync_user_to_real = () => {
    if (user_mirrors_real) {
      console.log(`user val sync back from ${user_val} to ${real_val}`)
      set_user_val(real_val)
    }
  }

  useEffect(sync_user_to_real, [real_val, user_mirrors_real])

  return [user_val, set_user_val]
}
