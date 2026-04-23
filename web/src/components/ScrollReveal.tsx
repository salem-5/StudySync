"use client";
import { useEffect, useRef, useState, ReactNode } from "react";

export default function ScrollReveal({ children, className = "" }: { children: ReactNode, className?: string }) {
    const [isVisible, setIsVisible] = useState(false);
    const ref = useRef<HTMLDivElement>(null);

    useEffect(() => {
        const observer = new IntersectionObserver(([entry]) => {
            if (entry.isIntersecting) {
                setIsVisible(true);
                observer.disconnect();
            }
        }, { threshold: 0.1 });

        if (ref.current) observer.observe(ref.current);
        return () => observer.disconnect();
    }, []);

    return (
        <div ref={ref} className={`reveal ${isVisible ? "active" : ""} ${className}`}>
            {children}
        </div>
    );
}