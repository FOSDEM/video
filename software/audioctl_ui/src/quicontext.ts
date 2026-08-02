import {QUI} from "rtui"
import {createContext} from "preact"
import {useEffect, useState} from "preact/hooks"


// rtui breaks Liskov Substitution
// const DemoQui: QUI = <QUI>{}

export const QuiContext = createContext<QUI | null>(null)

export const useQui = (handler: (e: any) => Promise<void>) => {
  const [qui, _] = useState(new QUI())

  useEffect(() => {
    qui.loop(handler)
  }, [])

  return qui
}