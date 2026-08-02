import {useContext, useEffect, useState, type Dispatch, type StateUpdater} from "preact/hooks"
import {QuiContext} from "./quicontext.ts"
import type {QUI} from "rtui"

interface ActionProps {
    bucket: string,
    reset_after?: number,
    action: () => void | Promise<void>
    syncBack: () => void | Promise<void>
}

function exec_action(qui: QUI | null, props: ActionProps) {
  if (qui) {
    qui.add({
      handler: async () => {
        await props.action()
      },
      bucket: props.bucket,
      reset_after: props.reset_after,
      reset: props.syncBack,
    })
  } else {
    props.action()
  }
}

export default function useReactive<T>(id: string, real: T, setReal: (v: T) => void, reset_after?: number): [T, Dispatch<StateUpdater<T>>] {
  const qui = useContext(QuiContext)
  const [user, setUser] = useState(real) // initial

  const [previous, setPrevious] = useState(real) // this only changes on frontend push

  const [forceSync, setForceSync] = useState(false)

  // on "user" value change, enqueue to qui
  useEffect(() => {
    // don't handle unchanged values or force pushes from the backend here
    if(forceSync || real === user) return

    exec_action(qui, {
      bucket: id,
      action: () => {
        setForceSync(false)
        setReal(user)
        setPrevious(user)
      },
      syncBack: () => setForceSync(true),
      reset_after: reset_after,
    })
  }, [user])

  // on real value change without user input
  useEffect(() => {
    if(previous == user || real != user) setForceSync(true)
  }, [real])

  // if force pushed, update UI value and unlock back
  useEffect(() => {
    if (!forceSync) return

    console.debug(`force sync from ${user} to ${real}`)
    setUser(real)
    setForceSync(false)
  }, [forceSync])

  return [user, setUser]
}