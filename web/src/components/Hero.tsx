"use client";
import { useRef } from "react";
import Terminal from "./Terminal";

export default function Hero() {
    const titleRef = useRef<HTMLHeadingElement>(null);

    const handleMouseMove = (e: React.MouseEvent<HTMLHeadingElement>) => {
        if (!titleRef.current) return;
        const rect = titleRef.current.getBoundingClientRect();
        const xPercent = ((e.clientX - rect.left) / rect.width) * 100;
        const yPercent = ((e.clientY - rect.top) / rect.height) * 100;

        titleRef.current.style.setProperty("--mouse-x", `${xPercent}%`);
        titleRef.current.style.setProperty("--mouse-y", `${yPercent}%`);
    };

    const handleMouseLeave = () => {
        if (!titleRef.current) return;
        titleRef.current.style.setProperty("--mouse-x", `50%`);
        titleRef.current.style.setProperty("--mouse-y", `50%`);
    };

    return (
        <section className="hero">
            <h1
                id="hero-title"
                ref={titleRef}
                onMouseMove={handleMouseMove}
                onMouseLeave={handleMouseLeave}
            >
                Sync your studies.
            </h1>
            <p>A native C++ environment built for speed. Manage tasks, collaborate with groups, and focus with zero friction.</p>

            <div className="hero-buttons">
                <a className="btn-primary" href="#downloads">
                    <i className="ph-bold ph-download-simple"></i> Download StudySync
                </a>
                <a className="btn-secondary" href="https://github.com/salem-5/StudySync" target="_blank">
                    <i className="ph-bold ph-github-logo"></i> Star on GitHub
                </a>
            </div>

            <Terminal />
        </section>
    );
}